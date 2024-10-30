#include <gdal_priv.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

void createYearlyRasterStacks(const std::string& inputFilePath) {
    // Register GDAL drivers
    GDALAllRegister();

    // Open the input raster stack (GRIB file)
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpen(inputFilePath.c_str(), GA_ReadOnly));
    if (dataset == nullptr) {
        std::cerr << "Failed to open dataset: " << inputFilePath << std::endl;
        return;
    }

    int layerCount = dataset->GetRasterCount();
    std::cout << "Total layers in the stack: " << layerCount << std::endl;

    // Define the start and end dates
    std::tm startDate = {0};
    startDate.tm_year = 1979 - 1900; // Year 1979
    startDate.tm_mon = 0;  // January
    startDate.tm_mday = 1; // Day 1
    std::time_t startTime = std::mktime(&startDate);

    std::tm endDate = {0};
    endDate.tm_year = 2023 - 1900; // Year 2023
    endDate.tm_mon = 11; // December
    endDate.tm_mday = 31; // Day 31
    std::time_t endTime = std::mktime(&endDate);

    // Compute the number of days in the range (start to end date)
    int totalDays = static_cast<int>(std::difftime(endTime, startTime) / (60 * 60 * 24)) + 1;

    // Generate date information for each layer
    std::vector<std::tm> dates;
    dates.reserve(totalDays);
    for (int i = 0; i < totalDays; ++i) {
        std::tm currentDate = startDate;
        currentDate.tm_mday += i;
        std::mktime(&currentDate);
        dates.push_back(currentDate);
    }

    // Extract unique years
    std::vector<int> years;
    for (const auto& date : dates) {
        int year = date.tm_year + 1900;
        if (years.empty() || year != years.back()) {
            years.push_back(year);
        }
    }

    // Create raster stack for each year
    for (int year : years) {
        std::vector<int> yearIndices;
        for (int i = 0; i < totalDays; ++i) {
            if (dates[i].tm_year + 1900 == year) {
                yearIndices.push_back(i + 1); // 1-based index for GDAL bands
            }
        }

        if (yearIndices.empty()) {
            continue;
        }

        // Create output raster for the year
        std::string outputFilename = "raster_stack_" + std::to_string(year) + ".tif";
        GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
        if (driver == nullptr) {
            std::cerr << "GTiff driver not available." << std::endl;
            return;
        }

        GDALDataset* outDataset = driver->Create(outputFilename.c_str(),
                                                 dataset->GetRasterXSize(),
                                                 dataset->GetRasterYSize(),
                                                 yearIndices.size(),
                                                 GDT_Float32,
                                                 nullptr);

        if (outDataset == nullptr) {
            std::cerr << "Failed to create output file: " << outputFilename << std::endl;
            return;
        }

        // Read and write the raster bands for the current year
        for (int j = 0; j < yearIndices.size(); ++j) {
            GDALRasterBand* band = dataset->GetRasterBand(yearIndices[j]);
            GDALRasterBand* outBand = outDataset->GetRasterBand(j + 1);

            int xSize = band->GetXSize();
            int ySize = band->GetYSize();

            std::vector<float> rasterData(xSize * ySize);
            CPLErr err = band->RasterIO(GF_Read, 0, 0, xSize, ySize, rasterData.data(), xSize, ySize, GDT_Float32, 0, 0);
            if (err != CE_None) {
                std::cerr << "Error reading raster band" << std::endl;
            }

            err = outBand->RasterIO(GF_Write, 0, 0, xSize, ySize, rasterData.data(), xSize, ySize, GDT_Float32, 0, 0);
            if (err != CE_None) {
                std::cerr << "Error writing raster band" << std::endl;
            }
        }

        // Clean up the output dataset
        GDALClose(outDataset);

        std::cout << "Raster stack for year " << year << " saved as " << outputFilename << std::endl;
    }

    // Clean up the input dataset
    GDALClose(dataset);
}

int main() {
    std::string inputFilePath = "/home/pedro/Desktop/arquivos/pessoas/jorge/sst/data.grib";
    createYearlyRasterStacks(inputFilePath);
    return 0;
}
