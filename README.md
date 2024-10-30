# Data Extraction and Analysis

This repository contains C++ and R code for extracting specific gridded data from NOAA's NCEP Reanalysis II and Copernicus Climate Data Store (CDS), as well as calculating maximum values and performing trend analysis using the Mann-Kendall method.

## Overview

The project includes the following components:

1. **C++ Code for NOAA NCEP Reanalysis II Data Extraction**
   - This code extracts `SKF.SFC.GAUSS` data from the NOAA website and calculates the maximum values.
   - [NOAA NCEP Reanalysis II Data](https://psl.noaa.gov/data/gridded/data.ncep.reanalysis2.html)

2. **C++ Code for Copernicus ERA5 Data Extraction**
   - This code extracts Sea Surface Temperature (SST) data from the Copernicus Climate Data Store and calculates the maximum values.
   - [Copernicus ERA5 Data](https://cds.climate.copernicus.eu/datasets/reanalysis-era5-single-levels?tab=download)

3. **R Code for Mann-Kendall Trend Calculation**
   - This R script performs the Mann-Kendall trend test on the extracted data, providing insights into the trend over time.

## Prerequisites

- C++ Compiler (e.g., g++)
- Kendall and raster packages for statistical and raster analysis
- Access to the respective datasets mentioned above

