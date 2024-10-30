#include <gdal_priv.h>
#include <iostream>
#include <vector>
#include <string>

// Function to check if a year is a leap year
bool isLeapYear(int year)
{
  if (year % 4 != 0)
    return false;
  if (year % 100 != 0)
    return true;
  if (year % 400 == 0)
    return true;
  return false;
}

// Function to extract the second layer (6 hrs) for each day and save to a new stack
void extractSecondLayerPerDay(const std::string &inputFilePath, const std::string &outputFilePath, int year)
{
  // Register GDAL drivers
  GDALAllRegister();

  // Open the input raster stack
  GDALDataset *dataset = static_cast<GDALDataset *>(GDALOpen(inputFilePath.c_str(), GA_ReadOnly));
  if (dataset == nullptr)
  {
    std::cerr << "Failed to open dataset: " << inputFilePath << std::endl;
    return;
  }

  // Get raster dimensions
  int rasterXSize = dataset->GetRasterXSize();
  int rasterYSize = dataset->GetRasterYSize();
  int totalLayers = dataset->GetRasterCount();

  // Determine the number of days in the year (365 or 366)
  int daysInYear = isLeapYear(year) ? 366 : 365;

  // Check if totalLayers matches the expected 4 layers per day
  if (totalLayers != daysInYear * 4)
  {
    std::cerr << "Unexpected number of layers in the dataset for year " << year << std::endl;
    GDALClose(dataset);
    return;
  }

  // Create output raster for the year
  GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
  if (driver == nullptr)
  {
    std::cerr << "GTiff driver not available." << std::endl;
    GDALClose(dataset);
    return;
  }

  // Create an output dataset with 365 or 366 layers
  GDALDataset *outDataset = driver->Create(outputFilePath.c_str(), rasterXSize, rasterYSize, daysInYear, GDT_Float32, nullptr);
  if (outDataset == nullptr)
  {
    std::cerr << "Failed to create output file: " << outputFilePath << std::endl;
    GDALClose(dataset);
    return;
  }

  // Extract only the second layer for each day (6 hrs) and write to the output dataset
  for (int day = 0; day < daysInYear; ++day)
  {
    int secondLayerIndex = day * 4 + 2; // The second layer for each day (6 hrs) is at index 2, 6, 10, ...
    GDALRasterBand *inBand = dataset->GetRasterBand(secondLayerIndex);
    GDALRasterBand *outBand = outDataset->GetRasterBand(day + 1);

    std::vector<float> rasterData(rasterXSize * rasterYSize);
    CPLErr err = inBand->RasterIO(GF_Read, 0, 0, rasterXSize, rasterYSize, rasterData.data(), rasterXSize, rasterYSize, GDT_Float32, 0, 0);
    if (err != CE_None)
    {
      std::cerr << "Error reading raster band " << secondLayerIndex << " for day " << day + 1 << std::endl;
    }

    err = outBand->RasterIO(GF_Write, 0, 0, rasterXSize, rasterYSize, rasterData.data(), rasterXSize, rasterYSize, GDT_Float32, 0, 0);
    if (err != CE_None)
    {
      std::cerr << "Error writing raster band " << day + 1 << " for year " << year << std::endl;
    }
  }

  // Close the datasets
  GDALClose(outDataset);
  GDALClose(dataset);

  std::cout << "Yearly raster stack for year " << year << " (only 6 hrs layers) saved as " << outputFilePath << std::endl;
}

int main()
{
  // Path to the folder containing input raster stacks
  std::string baseInputDir = "/home/pedro/Desktop/arquivos/pessoas/jorge/skt_sfc/";
  // Path to save output raster stacks
  std::string baseOutputDir = "/home/pedro/Desktop/arquivos/pessoas/jorge/skt_sfc/skt_sfc_temperature_6hrs/";

  // Loop through each year from 1979 to 2023
  for (int year = 1979; year <= 2023; ++year)
  {
    // Construct input file name using the pattern "YYYY_temperature.tif"
    std::string inputFilePath = baseInputDir + std::to_string(year) + "_temperature.tif";
    // Construct output file name
    std::string outputFilePath = baseOutputDir + "skt_sfc_temperature_6hrs_" + std::to_string(year) + ".tif";

    // Process the raster for the current year
    extractSecondLayerPerDay(inputFilePath, outputFilePath, year);
  }

  return 0;
}
