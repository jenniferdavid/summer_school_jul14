clear all
close all 
clc

i=0
CAMERA_X_RES=256
CAMERA_Y_RES=256

while(i<1000)
    i=i+1;
    figure(1)
    
    [ret,image]=simExtMatlabSM('vRepCamera69',int32(CAMERA_X_RES*CAMERA_Y_RES*3));
    rgbImage=flipdim(permute(reshape(uint8(image), 3,CAMERA_X_RES,CAMERA_Y_RES), [3 2 1]), 1); 
    subplot(2,2,1)
    imshow(rgbImage);figure(gcf);
    
    [ret,image]=simExtMatlabSM('vRepCamera71',int32(CAMERA_X_RES*CAMERA_Y_RES*3));
    rgbImage=flipdim(permute(reshape(uint8(image), 3,CAMERA_X_RES,CAMERA_Y_RES), [3 2 1]), 1); 
    subplot(2,2,2)
    imshow(rgbImage);figure(gcf);
    
    [ret,image]=simExtMatlabSM('vRepCamera72',int32(CAMERA_X_RES*CAMERA_Y_RES*3));
    rgbImage=flipdim(permute(reshape(uint8(image), 3,CAMERA_X_RES,CAMERA_Y_RES), [3 2 1]), 1); 
    subplot(2,2,3)
    imshow(rgbImage);figure(gcf);
    
    [ret,image]=simExtMatlabSM('vRepCamera73',int32(CAMERA_X_RES*CAMERA_Y_RES*3));
    rgbImage=flipdim(permute(reshape(uint8(image), 3,CAMERA_X_RES,CAMERA_Y_RES), [3 2 1]), 1); 
    subplot(2,2,4)
    imshow(rgbImage);figure(gcf);

end
