#!/bin/bash
# Test script for Producer-Consumer project

echo "==================================="
echo "TEST SUITE - Multiple Producers & Consumers"
echo "==================================="
echo ""

# Compile
echo "📦 Compiling..."
gcc -o program main.c queue.c producer.c consumer.c event.c -lpthread -Wall
if [ $? -ne 0 ]; then
    echo "❌ Compilation failed!"
    exit 1
fi
echo "✅ Compilation successful"
echo ""

# Test 1: Mode 1 (Manual - skip in automated test)
echo "Test 1: Mode 1 - Manual input (skipped in automated test)"
echo ""

# Test 2: Mode 2 - Auto with string
echo "Test 2: Mode 2 - Auto with string 'ssstq'"
echo "Config: 2 producers, 2 consumers, delay=0s"
echo "---"
echo -e '2\n2\n2\nssstq\n0' | timeout 10 ./program
echo "✅ Test 2 completed"
echo ""

# Test 3: Mode 3 - File input
echo "Test 3: Mode 3 - File input (input.txt)"
echo "Config: 3 producers, 2 consumers, delay=0s"
echo "---"
echo -e '3\n2\n3\ninput.txt\n0' | timeout 15 ./program
echo "✅ Test 3 completed"
echo ""

# Test 4: Mode 3 - Long file
echo "Test 4: Mode 3 - Long file (test_long.txt)"
echo "Config: 4 producers, 3 consumers, delay=0s"
echo "---"
echo -e '4\n3\n3\ntest_long.txt\n0' | timeout 20 ./program
echo "✅ Test 4 completed"
echo ""

echo "==================================="
echo "All tests completed!"
echo "==================================="
