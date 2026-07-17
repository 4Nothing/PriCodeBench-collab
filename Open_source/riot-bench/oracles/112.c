int fib_sr_get_route(fib_table_t *table, uint8_t *dst, size_t dst_size, kernel_pid_t *sr_iface_id,
                     uint32_t *sr_flags,
                     uint8_t *addr_list, size_t *addr_list_elements, size_t *element_size,
                     bool reverse, fib_sr_t **fib_sr)
{
    mutex_lock(&(table->mtx_access));

    if ((dst == NULL) || (sr_iface_id == NULL) || (sr_flags == NULL)
        || (addr_list == NULL) || (addr_list_elements == NULL) || (element_size == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    fib_sr_t *hit = NULL;
    fib_sr_t *tmp_hit = NULL;
    int check_free_entry = -1;

    bool skip = (fib_sr != NULL) && (*fib_sr != NULL)?true:false;
    /* Case 1 - check if we know a direct route */
    for (size_t i = 0; i < table->size; ++i) {

        if (fib_sr_check_lifetime(&table->data.source_routes->headers[i]) == -ENOENT) {
            /* expired, so skip this sr and remember its position */
            if (check_free_entry == -1) {
                /* we want to fill up the source routes from the beginning */
                check_free_entry = i;
            }
            continue;
        }

        if (skip) {
            if (*fib_sr == &table->data.source_routes->headers[i]) {
                skip = false;
            }
            /* we skip all entries upon the consecutive one to start search */
            continue;
        }

        size_t addr_size_match = dst_size << 3;
        if (universal_address_compare(table->data.source_routes->headers[i].sr_dest->address,
                                      dst, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            if (*sr_flags == table->data.source_routes->headers[i].sr_flags) {
                /* found a perfect matching sr, no need to search further */
                hit = &table->data.source_routes->headers[i];
                tmp_hit = NULL;
                if (check_free_entry == -1) {
                    check_free_entry = i;
                }
                break;
            }
            else {
                /* found a sr to the destination but with different flags,
                 * maybe we find a better one.
                 */
                tmp_hit = &table->data.source_routes->headers[i];
            }
        }
    }

    if (hit == NULL) {
        /* we didn't find a perfect sr, but one with distinct flags */
        hit = tmp_hit;
    }

    /* Case 2 - if no hit is found check if there is a matching entry in one sr_path
     * @note the first match wins, if we find one we will NOT continue searching,
     * since this search is very expensive in terms of compare operations
    */
    if (hit == NULL) {
        int error = 0;
        hit = _fib_create_sr_from_partial(table, dst, dst_size, check_free_entry, &error);
        if ((error != 0) && (error != -EHOSTUNREACH)) {
            /* something went wrong, so we clean up our mess
             *
             * @note we could handle -EHOSTUNREACH differently here,
             * since it says that we have a partial source route but no RP
             * to manage it.
             * That's why I let it pass for now.
             */
            if (hit != NULL) {
                hit->sr_lifetime = 0;

                if (hit->sr_path != NULL) {
                    fib_sr_entry_t *elt = NULL, *tmp = NULL;
                    LL_FOREACH_SAFE(hit->sr_path, elt, tmp) {
                        universal_address_rem(elt->address);
                        elt->address = NULL;
                        LL_DELETE(hit->sr_path, elt);
                    }
                    hit->sr_path = NULL;
                }
            }
            mutex_unlock(&(table->mtx_access));
            return error;
        }
    }

    /* Final step - copy the list in the desired order */
    if (hit != NULL) {

        /* store the current hit to enable consecutive searches */
        if (fib_sr != NULL) {
            *fib_sr = hit;
        }

        /* check the list size and if the sr entries will fit */
        int count;
        fib_sr_entry_t *elt = NULL;
        LL_COUNT(hit->sr_path, elt, count);

        if (((size_t)count > *addr_list_elements)
            || (sizeof(hit->sr_path->address->address) > *element_size)) {
            *addr_list_elements = count;
            *element_size = sizeof(hit->sr_path->address->address);
            mutex_unlock(&(table->mtx_access));
            return -ENOBUFS;
        }

        /* start copy the individual entries in the desired order */
        uint8_t *next_entry = addr_list;
        int one_address_size = *element_size;

        if (reverse) {
            /* we move to the last list element */
            next_entry += (count - 1) * sizeof(hit->sr_path->address->address);
            /* and set the storing direction during the iteration */
            one_address_size *= -1;
        }

        elt = NULL;
        LL_FOREACH(hit->sr_path, elt) {
            size_t tmp_size = sizeof(hit->sr_path->address->address);
            universal_address_get_address(elt->address, next_entry, &tmp_size);
            next_entry += one_address_size;
        }
        *sr_iface_id = hit->sr_iface_id;
        *sr_flags = hit->sr_flags;
        *addr_list_elements = count;
        *element_size = sizeof(hit->sr_path->address->address);
    }
    else {

        /* trigger RPs for route discovery */
        fib_signal_rp(table, FIB_MSG_RP_SIGNAL_UNREACHABLE_DESTINATION, dst, dst_size, *sr_flags);

        mutex_unlock(&(table->mtx_access));
        return -EHOSTUNREACH;
    }

    mutex_unlock(&(table->mtx_access));

    if (tmp_hit == NULL) {
        return 0;
    }
    else {
        return 1;
    }
}