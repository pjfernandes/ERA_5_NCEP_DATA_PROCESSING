#include <gdal_priv.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::max
#include <limits>    // For std::numeric_limits

void generateMaxskt_sfcTemperaturePerYear(const std::string &baseDir)
{
  GDALAllRegister();

  for (int year = 1979; year <= 2023; ++year)
  {
    // Construct the input file name for the raster stack in the specified directory
    std::string inputFileName = baseDir + "/skt_sfc_temperature_6hrs_" + std::to_string(year) + ".tif";

    // Open the yearly raster stack
    GDALDataset *dataset = static_cast<GDALDataset *>(GDALOpen(inputFileName.c_str(), GA_ReadOnly));
    if (dataset == nullptr)
    {
      std::cerr << "Failed to open dataset: " << inputFileName << std::endl;
      continue;
    }

    int xSize = dataset->GetRasterXSize();
    int ySize = dataset->GetRasterYSize();
    int layerCount = dataset->GetRasterCount();
    std::cout << "Processing year " << year << ": " << layerCount << " layers." << std::endl;

    // Create an array to store the max temperature values for each pixel
    std::vector<float> maxTemp(xSize * ySize, -std::numeric_limits<float>::infinity());

    // Iterate over each layer (each day of the year)
    for (int i = 1; i <= layerCount; ++i)
    {
      GDALRasterBand *band = dataset->GetRasterBand(i);
      std::vector<float> rasterData(xSize * ySize);

      // Read the current raster band (layer)
      CPLErr err = band->RasterIO(GF_Read, 0, 0, xSize, ySize, rasterData.data(), xSize, ySize, GDT_Float32, 0, 0);
      if (err != CE_None)
      {
        std::cerr << "Error reading raster band " << i << " of year " << year << std::endl;
        continue;
      }

      // Compute the maximum temperature for each pixel
      for (int j = 0; j < xSize * ySize; ++j)
      {
        maxTemp[j] = std::max(maxTemp[j], rasterData[j]);
      }
    }

    // Create output raster for max skt_sfc temperature
    std::string outputFilename = baseDir + "/max_skt_sfc_temperature/max_skt_sfc_temperature_" + std::to_string(year) + ".tif";
    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (driver == nullptr)
    {
      std::cerr << "GTiff driver not available." << std::endl;
      GDALClose(dataset);
      return;
    }

    GDALDataset *outDataset = driver->Create(outputFilename.c_str(), xSize, ySize, 1, GDT_Float32, nullptr);
    if (outDataset == nullptr)
    {
      std::cerr << "Failed to create output file: " << outputFilename << std::endl;
      GDALClose(dataset);
      return;
    }

    // Write the max temperature values to the output raster
    GDALRasterBand *outBand = outDataset->GetRasterBand(1);
    CPLErr err = outBand->RasterIO(GF_Write, 0, 0, xSize, ySize, maxTemp.data(), xSize, ySize, GDT_Float32, 0, 0);
    if (err != CE_None)
    {
      std::cerr << "Error writing output raster for year " << year << std::endl;
    }

    // Clean up
    GDALClose(outDataset);
    GDALClose(dataset);

    std::cout << "Max skt_sfc temperature raster for year " << year << " saved as " << outputFilename << std::endl;
  }
}

int main()
{
  // Base directory containing the input and output files
  std::string baseDir = "/home/pedro/Desktop/arquivos/pessoas/jorge/skt_sfc/skt_sfc_temperature_6hrs";
  generateMaxskt_sfcTemperaturePerYear(baseDir);
  return 0;
}
