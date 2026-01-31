#!/usr/bin/env python3
"""
Extract DAWG-only from a KWG file (which contains both DAWG and GADDAG).

KWG node format (32-bit little-endian):
  - bits 24-31: tile (letter 1-26)
  - bit 23: accepts flag (word ends here)
  - bit 22: is_end flag (last sibling in arc set)
  - bits 0-21: arc_index (pointer to next node)

Node 0's arc_index points to DAWG root.
Node 1's arc_index points to GADDAG root (we don't need this).
"""

import struct
import sys
from collections import deque

def read_kwg(filename):
    """Read KWG file and return list of uint32 nodes."""
    with open(filename, 'rb') as f:
        data = f.read()
    node_count = len(data) // 4
    nodes = list(struct.unpack(f'<{node_count}I', data))
    return nodes

def write_kwg(filename, nodes):
    """Write list of uint32 nodes to KWG file."""
    with open(filename, 'wb') as f:
        for node in nodes:
            f.write(struct.pack('<I', node))

def node_tile(node):
    return node >> 24

def node_accepts(node):
    return (node & 0x800000) != 0

def node_is_end(node):
    return (node & 0x400000) != 0

def node_arc_index(node):
    return node & 0x3FFFFF

def find_reachable_nodes(nodes, root_index):
    """
    Find all nodes reachable from the DAWG root.
    Returns a set of node indices that are part of the DAWG.
    """
    reachable = set()
    queue = deque([root_index])

    while queue:
        idx = queue.popleft()
        if idx == 0 or idx in reachable:
            continue

        # Process this arc set (consecutive nodes until is_end)
        i = idx
        while True:
            if i in reachable:
                break
            reachable.add(i)
            node = nodes[i]

            # Follow the arc if it points somewhere
            arc_idx = node_arc_index(node)
            if arc_idx != 0 and arc_idx not in reachable:
                queue.append(arc_idx)

            if node_is_end(node):
                break
            i += 1

    return reachable

def extract_dawg(input_file, output_file):
    print(f"Reading {input_file}...")
    nodes = read_kwg(input_file)
    print(f"  Total nodes: {len(nodes)}")

    # Get DAWG root from node 0
    dawg_root = node_arc_index(nodes[0])
    print(f"  DAWG root index: {dawg_root}")

    # Get GADDAG root from node 1 (for info)
    gaddag_root = node_arc_index(nodes[1])
    print(f"  GADDAG root index: {gaddag_root}")

    # Find all nodes reachable from DAWG
    print("Finding DAWG-reachable nodes...")
    dawg_nodes = find_reachable_nodes(nodes, dawg_root)
    print(f"  DAWG nodes: {len(dawg_nodes)}")

    # Create sorted list and mapping from old to new indices
    sorted_indices = sorted(dawg_nodes)
    old_to_new = {old_idx: new_idx + 1 for new_idx, old_idx in enumerate(sorted_indices)}
    # Index 0 is special (null/no arc), so we start mapping at 1

    # Build new node list
    # Node 0: pointer to new DAWG root (which will be at index 1 since that's where we map the old root)
    new_nodes = []

    # First node is pointer to DAWG root
    new_dawg_root = old_to_new[dawg_root]
    new_nodes.append(new_dawg_root)  # Just the arc_index, no tile or flags

    # Add all DAWG nodes with remapped arc indices
    for old_idx in sorted_indices:
        node = nodes[old_idx]
        tile = node_tile(node)
        accepts = node_accepts(node)
        is_end = node_is_end(node)
        old_arc = node_arc_index(node)

        # Remap the arc index
        if old_arc == 0:
            new_arc = 0
        else:
            new_arc = old_to_new.get(old_arc, 0)

        # Reconstruct the node
        new_node = (tile << 24)
        if accepts:
            new_node |= 0x800000
        if is_end:
            new_node |= 0x400000
        new_node |= new_arc

        new_nodes.append(new_node)

    print(f"Writing {output_file}...")
    write_kwg(output_file, new_nodes)

    # Report sizes
    old_size = len(nodes) * 4
    new_size = len(new_nodes) * 4
    print(f"  Original size: {old_size:,} bytes ({old_size/1024/1024:.2f} MB)")
    print(f"  New size: {new_size:,} bytes ({new_size/1024/1024:.2f} MB)")
    print(f"  Reduction: {(1 - new_size/old_size)*100:.1f}%")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.kwg> <output.kwg>")
        sys.exit(1)

    extract_dawg(sys.argv[1], sys.argv[2])
    print("Done!")
