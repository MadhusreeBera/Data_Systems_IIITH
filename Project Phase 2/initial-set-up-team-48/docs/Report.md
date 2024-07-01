# Data Systems
## Project Phase 1
## Team 48

### Assumptions
- We will be provided only square matrices.
- The page size is 1KB (1024 bytes)
- The input for the matrix commands will be matrices only.
- 1 Page can accomodate maximum of 256 integers, hence the function has accomodated maximum of 225 integers (15*15) values in a page (block).
- ***The block size is dynamic. If the value of the page size is increased, the implementation of the functions dynamically accomodates for it and handles the number of values in the blocks accordingly.***

### Commands
- LOAD
    - Input: A matrix in the form of a csv file.
    - Page Design: 
        - The matrix is broken into 15x15 submatrices and stored into different pages.
        - Temporary files in the temp folder are created to store the page wise distribution of the matrix.
        - The number of pages needed for storing an nxn matrix are : `ceil(n/15)*ceil(n/15)`
    - Implemented Here: [loadMatrix.cpp](../src/executors/loadMatrix.cpp)
- EXPORT
    - Input: An already loaded matrix.
    - The function reads the temporary files created for the particular matrix and writes them into the csv file.
    - Implemented Here: [exportMatrix.cpp](../src/executors/exportMatrix.cpp)
- TRANSPOSE
    - Input: An already loaded matrix.
    - Implementation:
        - Assume an n*n matrix needs to be transposed.
        - It is stored as `ceil(n/15)*ceil(n/15)` pages.
        - The each page is transposed separately and the entire matrix is transposed by renaming the temporary files with the corresponding files.
    - Implemented Here: [transposeMatrix.cpp](../src/executors/transposeMatrix.cpp)
- CHECKSYMMETRY
    - Input: An already loaded matrix.
    - Implementation:
        - We iterate over the matrix and check if matrix[i][j] is equal to matrix[j][i].
    - Implemented Here: [checksymmetry.cpp](../src/executors/checksymmetry.cpp)
- PRINT
    - Input: An already loaded matrix.
    - Implementation:
        - The temporary files are iterated over in the necessary order to print all the columns and 20 rows of the matrix.
    - Implemented Here: [printMatrix.cpp](../src/executors/printMatrix.cpp)
- COMPUTE 
    - Input: An already loaded matrix.
    - Implementation:
        - We check if the block is a diagonal block, if yes, we just subtract the block from its transpose,
        - In the other case, we find the block that would make the transpose of this block according to the transpose function, and then the necessary computation is performed.
    - Implemented Here: [compute.cpp](../src/executors/compute.cpp)
- RENAME
    - Input: An already loaded matrix and an unloaded matrix name.
    - Implementation:
        - The temporary files for the matrix are updated with the new name and the tableCatalogue also has a changed key corresponding to the matrix.
        - Once the renamed matrix is exported, the old csv file for the matrix is removed and is replaced by the renamed csv file.

- **For the `Transpose`, `Compute` and `Check Symmetry` function, maximum of two blocks are accessed at any step which satisfies the given requirements**

### Learnings from the Project:
- Optimized data stored
- Necessity based page designing
- Optimum solution building in terms of both space and time complexity
- Feature implementation in big codebases

### Project Contributors:
 - Madhusree
    - Load Function
    - Print Function
    - Export Function
    - Transpose Function
    - Compute Function
 - Anurag
    - Transpose Function
    - Debugging
    - Testing
    - Reporting
    - Disaster Handling
 - Sanya
    - Load Function
    - Transpose Function
    - Compute Function
    - Checksymmetry Function
    - Rename Function

- Co-ordination among team members mantained by regular discussion and code analysis.
