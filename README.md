# Readers and Writers Problem

This repository contains implementations of the classic **Readers and Writers** synchronization problem. The goal is to manage access to a shared resource such that:

- Multiple readers can access the resource simultaneously.
- Writers require exclusive access.

## Implemented Solutions

1. **Mutex and Condition Variables**  
    Uses mutexes and condition variables to synchronize access between readers and writers.
    In this one, the readers are prioritized over writers, meaning that if there are readers present, writers will wait until all readers have finished.

2. **Busy Wait**  
    Implements synchronization using busy waiting (spinlocks), where threads repeatedly check conditions.
    In this one, there is no prioritization between readers and writers, meaning that if there are both readers and writers present, they will compete for access to the resource.
    This approach is less efficient and can lead to high CPU usage, but it demonstrates a different synchronization technique.

3. **Barrier, Mutex, and Condition Variables**  
    Combines barriers, mutexes, and condition variables for more advanced synchronization.
    In this one, the writers are prioritized over readers, meaning that if there are writers waiting, readers will wait until all writers have finished.

## Usage

Each solution is implemented in its own source file. Refer to the code and comments for details on building and running each version.