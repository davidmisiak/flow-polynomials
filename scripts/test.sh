make build/main

for file in ./graphs/*; do
    echo "Testing $file"
    output1=$(python scripts/flow_poly.py < $file)
    output2=$(./build/main < $file)
    if [ "$output1" != "$output2" ]; then
        echo "Test failed"
        echo "Expected: $output1"
        echo "Got: $output2"
        exit 1
    fi
done
