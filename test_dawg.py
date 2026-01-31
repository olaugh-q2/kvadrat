#!/usr/bin/env python3
"""
Test the DAWG-only KWG file to verify word validation works correctly.
"""

import struct
import sys

def read_kwg(filename):
    """Read KWG file and return list of uint32 nodes."""
    with open(filename, 'rb') as f:
        data = f.read()
    node_count = len(data) // 4
    nodes = list(struct.unpack(f'<{node_count}I', data))
    return nodes

def node_tile(node):
    return node >> 24

def node_accepts(node):
    return (node & 0x800000) != 0

def node_is_end(node):
    return (node & 0x400000) != 0

def node_arc_index(node):
    return node & 0x3FFFFF

def letter_to_tile(c):
    """Convert letter to tile index (A=1, B=2, ..., Z=26)"""
    return ord(c.upper()) - ord('A') + 1

def is_valid_word(nodes, dawg_root, word):
    """Check if a word is valid in the DAWG."""
    if len(word) < 2:
        return False

    node_index = dawg_root
    for i, c in enumerate(word.upper()):
        letter = letter_to_tile(c)
        found = False
        accepts = False

        j = node_index
        while True:
            node = nodes[j]
            if node_tile(node) == letter:
                node_index = node_arc_index(node)
                accepts = node_accepts(node)
                found = True

                # If this is the last letter, check if word is complete
                if i == len(word) - 1:
                    return accepts
                break

            if node_is_end(node):
                return False
            j += 1

        if not found or node_index == 0:
            return False

    return False

def run_tests():
    print("Loading DAWG-only KWG file...")
    nodes = read_kwg('docs/CSW21-dawg.kwg')
    print(f"  Loaded {len(nodes)} nodes")

    # Get DAWG root from node 0
    dawg_root = node_arc_index(nodes[0])
    print(f"  DAWG root: {dawg_root}")

    # Test known valid words (CSW21 dictionary)
    valid_words = [
        'AA', 'AB', 'CAT', 'DOG', 'HELLO', 'WORLD', 'PYTHON',
        'TETRIS', 'WORD', 'GAME', 'LETTER', 'SCORE', 'PLAY',
        'QI', 'ZA', 'XI', 'XU',  # Short valid Scrabble words
        'AARDVARK', 'ZEBRA', 'QUIZ', 'JINX',
        'KWANZA', 'ZEPHYR', 'JAZZ', 'FIZZ', 'BUZZ',
        'QWERTY',  # Yes, this is valid in CSW21!
    ]

    # Test known invalid words/non-words
    invalid_words = [
        'A',       # Too short
        'XYZ',     # Not a word
        'ASDF',    # Not a word
        'ZZZZZ',   # Not a word
        'ABCDEF',  # Not a word
        'TTTTT',   # Not a word
        'QQQQQ',   # Not a word
    ]

    print("\nTesting valid words...")
    valid_passed = 0
    valid_failed = 0
    for word in valid_words:
        result = is_valid_word(nodes, dawg_root, word)
        if result:
            print(f"  ✓ {word}")
            valid_passed += 1
        else:
            print(f"  ✗ {word} (expected valid)")
            valid_failed += 1

    print("\nTesting invalid words...")
    invalid_passed = 0
    invalid_failed = 0
    for word in invalid_words:
        result = is_valid_word(nodes, dawg_root, word)
        if not result:
            print(f"  ✓ {word} (correctly rejected)")
            invalid_passed += 1
        else:
            print(f"  ✗ {word} (expected invalid, but accepted)")
            invalid_failed += 1

    print(f"\n=== Results ===")
    print(f"Valid words: {valid_passed}/{len(valid_words)} passed")
    print(f"Invalid words: {invalid_passed}/{len(invalid_words)} passed")

    total_passed = valid_passed + invalid_passed
    total_tests = len(valid_words) + len(invalid_words)

    if valid_failed == 0 and invalid_failed == 0:
        print(f"\n✓ All {total_tests} tests passed!")
        return 0
    else:
        print(f"\n✗ {valid_failed + invalid_failed} tests failed")
        return 1

def compare_with_original():
    """Compare DAWG-only file with original to verify word validation matches."""
    print("\n=== Comparing with original CSW21.kwg ===")

    try:
        orig_nodes = read_kwg('CSW21.kwg')
        dawg_nodes = read_kwg('docs/CSW21-dawg.kwg')
    except FileNotFoundError as e:
        print(f"  Skipping comparison: {e}")
        return 0

    orig_root = node_arc_index(orig_nodes[0])
    dawg_root = node_arc_index(dawg_nodes[0])

    # Test a larger set of words
    test_words = [
        'AA', 'AB', 'AD', 'AE', 'AG', 'AH', 'AI', 'AL', 'AM', 'AN',
        'CAT', 'DOG', 'HELLO', 'WORLD', 'PYTHON', 'TETRIS',
        'QUIZ', 'JINX', 'ZEPHYR', 'KWANZA',
        'AARDVARK', 'ZEBRA', 'XYLOPHONE',
        'QI', 'ZA', 'XI', 'XU', 'ZO',
        'XYZ', 'ASDF', 'QWERTY',  # Invalid
    ]

    mismatches = 0
    for word in test_words:
        orig_result = is_valid_word(orig_nodes, orig_root, word)
        dawg_result = is_valid_word(dawg_nodes, dawg_root, word)

        if orig_result != dawg_result:
            print(f"  MISMATCH: {word} - original={orig_result}, dawg-only={dawg_result}")
            mismatches += 1

    if mismatches == 0:
        print(f"  ✓ All {len(test_words)} words match between original and DAWG-only")
    else:
        print(f"  ✗ {mismatches} mismatches found")

    return mismatches

if __name__ == '__main__':
    result = run_tests()
    compare_result = compare_with_original()
    sys.exit(result or compare_result)
