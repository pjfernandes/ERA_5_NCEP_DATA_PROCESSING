# Data Extraction and Analysis

This repository contains C++ and R code for extracting specific gridded data from NOAA's NCEP Reanalysis II and Copernicus Climate Data Store (CDS), calculating maximum values, and performing trend analysis using the Mann-Kendall method.

## Overview

The project includes the following components:

1. **C++ Code for NOAA NCEP Reanalysis II Data Extraction**
   - **File:** `get_second_skt_sfc_gauss_second_layer.cpp`
     - Extracts `SKF.SFC.GAUSS` data from the NOAA website and calculates maximum values.
     - [NOAA NCEP Reanalysis II Data](https://psl.noaa.gov/data/gridded/data.ncep.reanalysis2.html)

   - **File:** `skt_sfc_gauss_second_layer.cpp`
     - Additional functionality for handling second layer data extraction.

2. **C++ Code for Copernicus ERA5 Data Extraction**
   - **File:** `sst.cpp`
     - Extracts Sea Surface Temperature (SST) data from the Copernicus Climate Data Store.
     - [Copernicus ERA5 Data](https://cds.climate.copernicus.eu/datasets/reanalysis-era5-single-levels?tab=download)

   - **File:** `sst_max.cpp`
     - Calculates maximum SST values from the extracted data.

3. **R Code for Mann-Kendall Trend Calculation**
   - **File:** `skt_sfc_trend.R`
     - Performs the Mann-Kendall trend test on the extracted data, providing insights into the trend over time.

## Prerequisites

- C++ Compiler (e.g., g++)
- R packages for statistical and raster analysis (e.g., `Kendall and raster packages`)
- Access to the respective datasets mentioned above

## Usage

### C++ Code

To run the C++ code:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/your-repo.git
   cd your-repo
