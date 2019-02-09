//
//int dSPAN = 15;
//
//int cropSIZE = 31;
//
///**
// * @brief Transform the input RGB matrix to a greyscale matrix, the R,G,B valuses are placed consecutively
// * @param
// * @param
// * @param
// */
//
//unsigned char* grayscale(unsigned char* ppm) {
//	printf("grayscaling../n");
//	int x, y;
//	char r, g, b;
//	unsigned char grayed[4096];
//	int size_x = *ppm++;
//	int size_y = *ppm++;
//	printf("The image is: %d x %d!! \n", size_x, size_y);
//	for(y = 0; y < size_y; y++)
//	for(x = 0; x < size_x; x++)
//	{
//		r = *ppm++; 	// R
//		g = *ppm++;	// G
//		b = *ppm++;	// B
//		*grayed = (r * 5 + g * 9 + b * 2) / 16;
//		grayed++;
//	}
//	printf("grayscaled!/n");
//	return grayed;
//
//}
//
///**
// * @brief cut the image to a small piece of given size
// * @param
// * @param
// * @param
// */
//
//unsigned char* crop(int cX, int cY, unsigned char* img) {
//	printf("cropping../n");
//	unsigned char cropped[cropSIZE * cropSIZE];
//	int x, y, i;
//	i = 0;
//	for(y = cY; y < cY + cropSIZE; y++)
//	for(x = cX; x < cX + cropSIZE; x++) {
//		cropped[i] = img[y * 64 + x];
//	}
//	printf("cropped!/n");
//	return cropped;
//}
////crop :: [Int] -> Matrix Double -> Matrix Double
////crop [cX,cY] img = cropMat cropSIZE cropSIZE cX cY img
////
////
////
/////**
//// * @brief
//// * @param
//// * @param
//// * @param
//// */
////xcorr2 :: Matrix Double -> Matrix Double -> Matrix Double
////xcorr2 pat img = mapMat (reduceMat (+)) products
////  where
////    products    = mapMat timesP stencils
////    timesP mat  = zipWithMat (*) pat mat
////    stencils    = stencilMat patW patH img
////    (patW,patH) = sizeMat pat
////
////
/////**
//// * @brief get the position of the image piece which has the biggest grayscale sum
//// * @param
//// * @param
//// * @param
//// */
////posMax :: Matrix Double -> [Int]
////posMax img = [cX, cY]
////  where
////    ((cX,cY),_) = reduceMat findMax $ zipMat indexMat img
////    findMax (c1,v1) (c2,v2) | v1 >= v2  = (c1,v1)
////                            | otherwise = (c2,v2)
////
////
/////**
//// * @brief calculate the coordinates of the next cropping point according to the previous object position
//// * @param
//// * @param
//// * @param
//// */
////calcCoord :: [Int] -> Matrix Double -> [Int]
////calcCoord [prevX,prevY] img = [cropX,cropY]
////  where
////    cropX | prevX <= dSPAN     = 0
////          | prevX > imgW-dSPAN = imgW - cropSIZE - 1
////          | otherwise          = prevX - dSPAN - 1
////    cropY | prevY <= dSPAN     = 0
////          | prevY > imgH-dSPAN = imgH - cropSIZE - 1
////          | otherwise          = prevY - dSPAN - 1
////    (imgW,imgH) = sizeMat img
////
////
/////**
//// * @brief
//// * @param
//// * @param
//// * @param
//// */
////objectPos :: [Int] -> [Int] -> [Int]
////objectPos [cropX,cropY] [offsetX,offsetY]
////  = [cropX+offsetX+2, cropY+offsetY+2]
