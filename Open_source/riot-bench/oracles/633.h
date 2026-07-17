static inline int ut_process_str_expand(const char *ut,
                                        const ut_process_var_t *vars,
                                        size_t vars_len,
                                        char *uri, size_t *uri_len)
{
    return ut_process_expand(ut, strlen(ut), vars, vars_len, uri, uri_len);
}
