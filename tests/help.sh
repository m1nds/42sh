(echo a; lss)
while if false; 
then echo b;
elif true;
then echo a; false;
else echo b;
fi;
do echo helpme;
done;
for test in a b c d e;
do echo $test | cat -e > helpme;
done;
while false;
do echo a;
done;
until ! false;
do echo c;
done;
a=b; echo $a;
test(){ ls && exit 2 || ls;};
(test);
cat < helpme; echo a >> helpme; ls >& 2 <& 3 <> helpme;
cat < helpme; echo a >> helpme; ls < helpme;
unset a; export a; a=b; export a; unset a;
cd ../; echo $PWD; . file; test; { echo b; };
echo "ab"c'def$e'$a;
echo HEY GUYS;





