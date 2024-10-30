#include <gdal_priv.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::max

void generateMaxSSTPerYear(const std::string &baseDir)
{
  GDALAllRegister();

  for (int year = 1979; year <= 2023; ++year)
  {
    // Construct the input file name for the raster stack
    std::string inputFileName = baseDir + "/raster_stack_" + std::to_string(year) + ".tif";

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

    // Create an array to store the max SST values for each pixel
    std::vector<float> maxSST(xSize * ySize, -std::numeric_limits<float>::infinity());

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

      // Compute the maximum SST for each pixel
      for (int j = 0; j < xSize * ySize; ++j)
      {
        maxSST[j] = std::max(maxSST[j], rasterData[j]);
      }
    }

    // Create output raster for max SST
    std::string outputFilename = baseDir + "/max_sst_" + std::to_string(year) + ".tif";
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

    // Write the max SST values to the output raster
    GDALRasterBand *outBand = outDataset->GetRasterBand(1);
    CPLErr err = outBand->RasterIO(GF_Write, 0, 0, xSize, ySize, maxSST.data(), xSize, ySize, GDT_Float32, 0, 0);
    if (err != CE_None)
    {
      std::cerr << "Error writing output raster for year " << year << std::endl;
    }

    // Clean up
    GDALClose(outDataset);
    GDALClose(dataset);

    std::cout << "Max SST raster for year " << year << " saved as " << outputFilename << std::endl;
  }
}

int main()
{
  std::string baseDir = "/home/pedro/Desktop/arquivos/pessoas/jorge";
  generateMaxSSTPerYear(baseDir);
  return 0;
}
