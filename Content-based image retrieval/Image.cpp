#include "Image.h"

void Image::assignImg(String count, Mat image) {
	Id = count;
	content = image;
}

Mat Image::getImg() {
	return content;
}

String Image::getId() {
	return Id;
}

void Image::showImg() {
	// Resize the image content to 256x192
	Mat resizedContent;
	resize(content, resizedContent, Size(256, 192));

	// Display the resized image
	imshow(Id, resizedContent);
}