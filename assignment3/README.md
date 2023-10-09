# Assignment 3

## Strategy 1
- Read CLI
- Global acces for all threads
  - result struct (**ResultStruct)
  - Write queue
  - row index
  - row index mutex
  - row size
  - work done array
- In main thread
  - Create calc slaves
  - write to file
  - Wait for signal before writing
- calc slave
  - Works on rows
  - loop over each point in the row
  - Mutex for getting a row index
  - send signal when done and set work done to 1
