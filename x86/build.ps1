$opts = @("O0","O1","O2","O3")
foreach ($o in $opts) {
    g++ -S -std=c++20 -$o strlen.cpp -o "strlen_$o.s"
}