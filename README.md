# TripSync — AVL-Based Trip & Itinerary Planner

A command-line trip planning system written in **C**, built around a 3-level nested data model that combines a singly linked list with two independent self-balancing AVL trees. Designed to manage multiple trips, each containing multiple activities, each of which can have its own multi-step navigation route — with all core CRUD operations, search, and file persistence implemented from scratch (no external libraries).

## Why this project

Most itinerary/task-tracking demos use flat arrays or simple linked lists, which degrade to O(n) search as data grows. TripSync instead uses **AVL trees** for both activities (keyed by date) and navigation steps (keyed by step ID), guaranteeing O(log n) insert, delete, and search even as a trip grows large — while a linked list ties multiple trips together at the top level.

## Data Model

```
TripList (Linked List)
 └── Trip
      └── TripNode (AVL Tree, keyed by date)
           └── Activity (hotel / flight / visit)
                └── NavNode (AVL Tree, keyed by step ID)
                     └── Navigation step (direction, distance, cost)
```

## Features

- **Trip management** — add, list, select, and delete trips (singly linked list)
- **Activity management** — full CRUD on activities within a trip, stored in a self-balancing AVL tree keyed by date
- **Navigation sub-routes** — each activity can hold its own AVL tree of turn-by-turn navigation steps, with cost/distance tracking
- **Self-balancing AVL trees** — independently implemented for both entity types, including all four rotation cases (LL, RR, LR, RL), used for insert *and* delete
- **Date-range search with subtree pruning** — range queries use the BST ordering property to skip irrelevant subtrees instead of scanning every node
- **Path finding** — locate a path between a source and destination activity within a trip, with aggregated cost across visited-only segments
- **Duplicate location detection** — flags locations visited more than once across a trip
- **Cost analytics** — sort hotel activities by cost, compute total trip cost live (including nested navigation costs)
- **File persistence** — custom pipe-delimited serialization format for saving/loading the entire linked-list-of-trees structure to disk, with duplicate-ID protection on reload
- **Full memory management** — recursive cleanup cascades correctly through nested trees on delete and program exit

## Build & Run

```bash
gcc -Wall -o tripsync tripsync.c
./tripsync
```

## Example Usage

```
============  MAIN MENU  ============
 1. Add New Trip
 2. View All Trips
 3. Select & Manage a Trip
 4. Delete a Trip
 5. Save All Data to File
 6. Load Data from File
 7. Range Search (by Date)
 0. Exit
```

Within a trip, activities can be added with nested navigation steps on the spot, or navigation can be attached to an existing activity later via the navigation sub-menu.

## Design Notes

- **Why AVL over a plain BST?** Trip activities are frequently inserted and removed as plans change; an unbalanced BST could degrade to a linked list (O(n)) in the worst case (e.g., activities added in date order). AVL rebalancing guarantees O(log n) height at all times.
- **Why two separate AVL implementations instead of generic/void\* trees?** Keeps the code explicit and readable for a systems-level C project without the complexity of generic programming via void pointers — a deliberate trade-off favoring clarity.
- **Pruned range search** — rather than an inorder traversal of the whole tree, `collectRange()` only recurses into a subtree when it could plausibly contain values within `[d1, d2]`, based on BST ordering.

## Tech Stack

C · AVL Trees · Linked Lists · Recursion · File I/O

## Possible Future Improvements

- Replace fixed-size arrays (`arr[100]`) with dynamic memory for scalability
- Add input validation/sanitization for file loading
- Export trip summaries to CSV