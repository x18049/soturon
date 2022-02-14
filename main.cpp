 
//
//  main.cpp
//  openCV_4
//
//  Created by 加藤祥真 on 2021/05/13.
//

//コンパイルコマンド　g++ main.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#include <vector>


//vectorから平均画像を返す関数
cv::Mat productAveImage(cv::Size movie_size, std::vector<cv::Mat> frame_vec){
    //エラー処理
    if(frame_vec.size() < 1){
        std::cout << "Empty images." << std::endl;
        cv::Mat zero_img = cv::Mat::zeros(300, 300, CV_8UC3);
        return (zero_img);
    }
    
    int img_size = movie_size.width * movie_size.height;//動画サイズを取得
    int frame_counter = 0;
    
    //平均画像用の領域確保
    cv::Mat ave_img = cv::Mat::zeros(movie_size, CV_8UC3);
    std::vector<int> R(img_size, 0), G(img_size, 0), B(img_size, 0);
    
    for(cv::Mat src_img:frame_vec){
        for(int i=0; i<src_img.rows; i++){
            cv::Vec3b *src = src_img.ptr<cv::Vec3b>(i);
            for(int j=0; j<src_img.cols; j++){
                B[i*(src_img.cols)+j] += src[j][0];
                G[i*(src_img.cols)+j] += src[j][1];
                R[i*(src_img.cols)+j] += src[j][2];
            }
        }
        
        frame_counter++;//シーンの総フレーム数を記録
    }
    
    //平均画像の生成
    for(int i=0; i<ave_img.rows; i++){
        cv::Vec3b *ave = ave_img.ptr<cv::Vec3b>(i);
        for(int j=0; j<ave_img.cols; j++){
            ave[j][0] = B[i*(ave_img.cols)+j]/frame_counter;
            ave[j][1] = G[i*(ave_img.cols)+j]/frame_counter;
            ave[j][2] = R[i*(ave_img.cols)+j]/frame_counter;
        }
    }
    return (ave_img);
}

int main (int argc, const char* argv[])
{
    //内蔵カメラからビデオキャプチャ"capture"生成
    cv::VideoCapture capture("/Users/x18049xx/卒業研究/動画/wtt3-3_trim.mp4");  //"water.mov"にすればムービーファイルから映像を取り込む
    
    // 動画のfps
    int count = capture.get(cv::CAP_PROP_FRAME_COUNT);
    int frame_coutn=0;

    printf("%d\n",count);
    
    //キャプチャできたかチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    cv::Mat src_img, org_img,sourceImage;  //オリジナルの入力画像格納用（大抵大きすぎる）
    cv::Size movie_size;//映像のサイズを取得
    std::vector<cv::Mat> src_list;//入力記録用
    
    capture >> sourceImage;
    while(1){
        capture >> src_img;
        org_img = src_img.clone();
        
        if(src_img.empty()){
            break;
        }
        
        frame_coutn ++;
//        printf("%d\n",frame_coutn);
        //動画サイズの確保
        if(movie_size.empty()){
            movie_size = src_img.size();
        }
        
        src_list.push_back(org_img);//入力画像を逐次記録
    }
    
    cv::Mat ave_img;
    ave_img = productAveImage(movie_size, src_list);
    

//    cv::namedWindow("ave_img"); //ウィンドウの生成
//    cv::imshow("ave_img", ave_img); //ウィンドウに画像を表示
    cv::imwrite("ave/ave_img3-3.jpg", ave_img);


    cv::waitKey(0);
    
    
    
    return 0;
}
