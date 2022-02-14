//
//  edge.cpp
//  
//
//  Created by x18049xx on 2021/06/07.
//

#include <stdio.h>
#include <opencv2/opencv.hpp> //OpenCV 関連ヘッダ


//コンパイル g++ tabledetection.cpp -std=c++11 `pkg-config --cflags --libs opencv4`

int main (int argc, const char* argv[])
{
    
    //格納用変数
    cv::Mat sourceImage = cv::imread("ave/ave_img6.jpg");
    cv::Mat FilterImage(sourceImage.size(), CV_8UC3);
    cv::Mat grayImage(sourceImage.size(),CV_8UC1);
    cv::Mat edgeImage(sourceImage.size(), CV_8UC1);//エッジ検出
    cv::Mat rectImage(sourceImage.size(), CV_8UC3); //卓球台切り抜き
    cv::Mat test;
    
    
//    動画用格納変数
    cv::VideoCapture capture("/Users/x18049xx/卒業研究/動画/wtt6_trim.mp4"); //動画入力
    
    cv::VideoCapture capture_mask("mask-RCNN/wtt6_trim_mask_rcnn_out_py.mp4");//"water.mov"にすればムービーファイルから映像を取り込む
    //カメラがオープンできたかどうかをチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
    return -1;
    }
    
    if (capture_mask.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    

     cv::namedWindow("Frame");
     cv::moveWindow("Frame", 0,0);
    
    // 幅
    int capture_W = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    // 高さ
    int capture_H = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    


     cv::Mat originalImage;
     
    
    
    //ラベリング用変数
    cv::Mat LabelImg;
    cv::Mat stats;
    cv::Mat centroids;
    
    
    cv::Mat Mask_rcnnImage;
    cv::Mat sourceImage_mask;
    cv::Mat Mask_rcnnImage_bin;
    cv::Mat result_img;
    
    //画像の中心
    double centerX,centerY;
    double X,Y;


    //画像がなかったら終了
    if(sourceImage.data == 0){
        printf("画像が存在しません");
        exit(0);
    }
    
    //バイラテリフィルタ
//    cv::bilateralFilter(sourceImage,FilterImage, 25,70,100);
    cv::bilateralFilter(sourceImage,FilterImage, 9,230,230);

    
    //grayスケール化
    cv::cvtColor(FilterImage,grayImage,cv::COLOR_BGR2GRAY);
    
    //エッジ生成
    cv::Canny(grayImage,edgeImage,20,250,3);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::dilate(edgeImage, edgeImage, element, cv::Point(-1,-1), 1);



    //画像の中心
    centerX = sourceImage.cols/2;
    centerY = sourceImage.rows/2;
    
    X = sourceImage.cols;
    Y = sourceImage.rows;
    
    printf("%f\n",X);
    printf("%f\n",Y);


    
    //ラベリング処理
    int nLab = cv::connectedComponentsWithStats(edgeImage, LabelImg, stats, centroids);

    // ラベリング結果の描画色を決定
    std::vector<cv::Vec3b> colors(nLab);
    colors[0] = cv::Vec3b(0, 0, 0);
    for (int i = 1; i < nLab; ++i) {
        colors[i] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));

    }


    // ラベリング結果の描画
       cv::Mat Dst(sourceImage.size(), CV_8UC3);
       for (int i = 0; i < Dst.rows; ++i) {
           int *lb = LabelImg.ptr<int>(i);
           cv::Vec3b *pix = Dst.ptr<cv::Vec3b>(i);
           for (int j = 0; j < Dst.cols; ++j) {
               pix[j] = colors[lb[j]];
           }
       }
    
    
    //全ての領域の重心と中心の差を出す
    double minX[nLab],minY[nLab],x3,y3,min[nLab],min2=0,mx=0,my=0;
    int count=0;
    
    //中央値を出す
    double median,area2[nLab],tmp;
    for (int i = 1; i < nLab; ++i) {
        int *param = stats.ptr<int>(i);

        int area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];

        area2[i] = area;

    }

    //area2のソート
    for (int i = 1; i < nLab; ++i) {
        for(int j=i+1; j < nLab; j++){
            if(area2[i] > area2[j]){
                tmp = area2[i];
                area2[i] = area2[j];
                area2[j] = tmp;
            }
        }
    }
        //中央値の計算
//        if(nLab%2 == 0){
//            median = (area2[nLab/2] + (area2[(nLab/2)+1]))/2;
//        }else if (nLab%2 == 1){
//            median = area2[nLab/2];
//        }
//    printf("%d",nLab);
//    printf("%f",area2[nLab/2]);
//    printf("%f",median);

    
    
    
    
    for (int i = 1; i < nLab; ++i) {
        int *param = stats.ptr<int>(i);
        double *param2 = centroids.ptr<double>(i);

   

        //重心
        int x = static_cast<int>(param2[0]);
        int y = static_cast<int>(param2[1]);
        
        min[i] = sqrt((x - centerX)*(x - centerX)+ (y - centerY)*(y-centerY));
        printf("%d:%f\n", i,min[i]);
       

            
    }
    
    //中心と近い領域を抽出
    min2 = min[1];
    for (int i = 2; i < nLab; ++i) {
        if(min2 > min[i]){
            count = i;
            min2 = min[i];
            printf("%d\n",count);

        }
    }
    

    double tableX=0,tableY=0;
    double rectx,recty,recth;
    
    double width_M = 0,height_M = 0;
    cv::Point2f bound;


//       ROIの設定
   for (int i = 1; i < nLab; ++i) {
       int *param = stats.ptr<int>(i);
       double *param2 = centroids.ptr<double>(i);


       int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
       int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
       int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
       int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
       
       

       //面積
       int area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];

       //重心
       int x2 = static_cast<int>(param2[0]);
       int y2 = static_cast<int>(param2[1]);
       std::stringstream num;

       num << i;

           cv::circle(Dst,cv::Point(x2, y2), 8, cv::Scalar(0, 0, 255), -1);
           cv::circle(Dst,cv::Point(centerX, centerY), 8, cv::Scalar(255, 0, 0), -1);
       if(i == count){
//           cv::rectangle(Dst, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 2);
           bound.x = x;
//           bound.y = y;

           bound.y = abs(y - (height/2));
           width_M = width;
           height_M = abs(height + (height/2));
//           height_M = height;

           cv::Rect roi(cv::Point(x,y), cv::Size(width, height));
           rectImage = sourceImage(roi); // 切り出し画像

       }

//       cv::putText(Dst, num.str(), cv::Point(x, y), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
//       cv::rectangle(Dst, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 2);
       
//       cv::imshow("Dst",Dst);
//       cv::imshow("LabelImg",LabelImg);

   }
       

    
    //動画の最大フレーム数
    int max_frame = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
    // 動画のfps
    double fps = capture.get(cv::CAP_PROP_FPS);
    
//    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    
//    printf("fps:%f",fps);

    
//    cv::VideoWriter writer("output2/output_wtt3-1_mask2.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, cv::Size(width_M, height_M));
    
    cv::VideoWriter writer("output2/output_wtt6.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, cv::Size(capture_W, capture_H));

  
    cv::imshow("Dst",Dst);
    cv::waitKey();
    
   while(1){
       //(a)ビデオキャプチャから1フレーム"capture"を取り込んで，"originalImage"を生成
       capture >> originalImage;
       capture_mask >> Mask_rcnnImage;

       //ビデオが終了したら無限ループから脱出
       if (originalImage.data==NULL) {
           break;
       }else if (Mask_rcnnImage.data==NULL) {
           break;
       }
       //マスク処理
//       cv::bitwise_and(originalImage,Mask_rcnnImage,sourceImage_mask);

       //Mask_rcnnImageが3チャンネル画像なためグレースケール化して二値化
        cv::cvtColor(Mask_rcnnImage, Mask_rcnnImage_bin, cv::COLOR_BGR2GRAY); //グレースケールに変換
        cv::threshold(Mask_rcnnImage_bin,Mask_rcnnImage_bin,50,255,cv::THRESH_BINARY);
               
       originalImage.copyTo(result_img);
        //貼り付け
       sourceImage.copyTo(result_img,Mask_rcnnImage_bin);
       
//       cv::GaussianBlur(result_img, result_img, cv::Size(5, 5), 1, 1);

       
       
       cv::Rect roi(cv::Point(bound.x,bound.y), cv::Size(width_M, height_M));
//       rectImage = originalImage(roi); // 切り出し画像
       rectImage = result_img(roi); // 切り出し画像

       //"originalImage"をリサイズして"frameImage"生成
       
//       cv::imshow("original", originalImage);
//       cv::imshow("rectImage", rectImage);
       cv::imshow("result",rectImage);
//       cv::imshow("sourceImage_mask",sourceImage_mask);

       
       writer << result_img;  // 画像 image を動画ファイルへ書き出す
       int key = cv::waitKey(30);
       if (key=='q' || key=='Q'){
           break;
       }
   }
    
    
    return 0;
}
