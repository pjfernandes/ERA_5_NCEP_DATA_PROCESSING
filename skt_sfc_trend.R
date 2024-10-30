library(raster)
library(Kendall)

setwd("/home/pedro/Desktop/arquivos/pessoas/jorge/skt_sfc/skt_sfc_temperature_6hrs/max_skt_sfc_temperature/")

arquivos <- dir(pattern="max_skt_sfc.*.tif$")

lista <- list()

for (arquivo in arquivos) {
  r <- raster(arquivo)
  lista <- append(lista, r)
}

s <- stack(lista)

fun1<- function(x) { 
  #dat<-1:365
  if (is.na(x[1])) { 
    NA 
  } else { 
    m=MannKendall(x)
    m$tau[1]
  } 
} 

fun2 <- function(x) { 
  #dat<-1:365
  if (is.na(x[1])) { 
    NA 
  } else { 
    m=MannKendall(x)
    m$sl[1]
  } 
} 


l_m <- calc(s, fun1)
p <- calc(s, fun2)

plot(l_m)

p_mask<-p<0.05
p_mask[p_mask==0][]<-NA

final<-l_m*p_mask

plot(final)
##########################
r <- stack("/home/pedro/Desktop/arquivos/pessoas/jorge/skt_sfc/1979_temperature.tif")

plot(r[[1]])

extent(r[[1]])

convert_longitude <- function(lon) {
  lon_180 <- ifelse(lon > 180, lon - 360, lon)
  return(lon_180)
}

extent(final) <- extent(convert_longitude(304.6875) ,convert_longitude(330.9375), -34.27432, 0) 

plot(final)
writeRaster(final, "skt_sfc_mk_trend.tif")

