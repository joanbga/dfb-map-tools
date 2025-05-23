# DFB Map Utils

A command-line tool for analyzing binary map files and performing various operations on map cells.

## Description

Map Utils reads binary map files and provides functionality to analyze cell connectivity and properties. It currently supports two main commands for neighbor analysis and adjacent free space verification.

## Compilation

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

*Note: Adjust include and library paths according to your project structure*

## Usage

```bash
./map_utils <binary_file_path> <command> [arguments...]
```

### Required Arguments

- `<binary_file_path>` : Path to the binary map file to analyze

### Available Commands

#### `hasFourAdjacentCellsFree`

Checks if a given cell has four adjacent free cells.

**Syntax:**
```bash
./map_utils <binary_file> hasFourAdjacentCellsFree <cellId> [occupied_cells...]
```

**Parameters:**
- `<cellId>` : ID of the cell to check
- `[occupied_cells...]` : Optional list of cell IDs considered as occupied

**Output:**
- `OK` : The cell has four adjacent free cells
- `KO` : The cell does not have four adjacent free cells

**Return Code:**
- `0` : Success (four adjacent free cells)
- `1` : Failure (less than four adjacent free cells or error)

**Example:**
```bash
./map_utils map.bin hasFourAdjacentCellsFree 100
./map_utils map.bin hasFourAdjacentCellsFree 100 50 75 120
```

#### `getNeighbors`

Displays neighboring cells of a given cell with their properties.

**Syntax:**
```bash
./map_utils <binary_file> getNeighbors <cellId>
```

**Parameters:**
- `<cellId>` : ID of the cell whose neighbors you want to retrieve

**Output:**
For each neighboring cell:
```
<neighborId>: <direction> <Free|Not Free>
```

**Return Code:**
- `0` : Success
- `1` : Error (non-existent cell or other error)

**Example:**
```bash
./map_utils map.bin getNeighbors 100
```

Example output:
```
99: West Free
101: East Not Free
150: North Free
50: South Free
```

