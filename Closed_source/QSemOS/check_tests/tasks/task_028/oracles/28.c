[CASE] none active -> false
  expected=false
  actual  =false
  result  =PASS

[CASE] one running -> true
  expected=true
  actual  =true
  result  =PASS

[CASE] expired -> true
  expected=true
  actual  =true
  result  =PASS

[CASE] all free -> false
  expected=false
  actual  =false
  result  =PASS

[CASE] created ignored -> false
  expected=false
  actual  =false
  result  =PASS

Pass-Rate: 100.00% (5/5)
