$opts = @("O0","O1","O2","O3")
foreach ($o in $opts) {
    g++ -S -std=c++20 -mavx2 -$o test.cpp -o "strlen_$o.s"
}