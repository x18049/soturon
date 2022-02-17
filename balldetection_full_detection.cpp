//
//  Ball detection Ball detection ball detection.cpp
//  
//
//  Created by x18049xx on 2021/09/01.
//
//コンパイル g++ balldetection_full_detection.cpp -std=c++11 `pkg-config --cflags --libs opencv4`

#include <stdio.h>
#include<math.h>
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

cv::Point2f mouse_click;
//マウスクリック格納
int count_project = 0;
//射影変換用行列
cv::Point2f translate[4];


//白検出
cv::Mat white_mask(cv::Mat src_img)
{
    cv::Mat hsv_img,mask;
    
    cv::cvtColor(src_img, hsv_img, cv::COLOR_BGR2HSV); //hsvに変換
    
    // inRangeを用いて白検出
    cv::Scalar s_min = cv::Scalar(0, 0, 100);
//        cv::Scalar s_max = cv::Scalar(180, 45, 255);
    cv::Scalar s_max = cv::Scalar(180, 55, 255);

    cv::inRange(hsv_img, s_min, s_max, mask);
    
    cv::imshow("HsvImage",mask);
    
    cv::threshold(mask,mask,50,255,cv::THRESH_BINARY);

    
//    5x5 の矩形構造要素"element"の生成
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
     //"binImage"に対して構造要素"element"を用いて収縮処理を 1 回適用して"binImage"に出力
//    cv::dilate(mask, mask, element, cv::Point(-1,-1), 2);
    cv::dilate(mask, mask, element, cv::Point(-1,-1), 1);
    
        return (mask);
}

//差分
cv::Mat absdiff_mask(cv::Mat src_img,cv::Mat backImage, int W, int H)
{
//    cv::Mat backImage(cv::Size(W,H),CV_8UC3);
    cv::Mat subImage(cv::Size(W,H),CV_8UC3);
    cv::Mat gray,binary;
    //nとn-1番目フレームの差分
    cv::absdiff(src_img, backImage, subImage);
    
    
//    backImage = src_img.clone();

    
    cv::cvtColor(subImage, gray, cv::COLOR_BGR2GRAY); //グレースケールに変換

   
    
    //grayを二値化
    cv::threshold(gray,binary,50,255,cv::THRESH_BINARY);
    
    //5x5 の矩形構造要素"element"の生成
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
    //ピン球の領域が小さかったので膨張
//    cv::dilate(binary, binary, element, cv::Point(-1,-1), 2);
    cv::dilate(binary, binary, element, cv::Point(-1,-1), 1);


    return (binary);
}



////マウスイベント
//void mouse_callback(int event, int x, int y, int flags, void *userdata)
//{
//    if (event ==  cv::EVENT_LBUTTONDOWN) {
//
//        std::cout << "(" << x << ", " << y << ")" <<  std::endl;
//        mouse_click = cv::Point2f(x, y);
//        translate[count_project] = mouse_click;
//        count_project ++;
//    }
//}


int main (int argc, const char* argv[])
{
    cv::VideoCapture capture("output/output_wtt11_mask.mp4");//"water.mov"にすればムービーファイルから映像を取り込む
    //キャプチャできたかチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    
//    cv::Mat sourceImage = cv::imread("avave_img10.jpg");
    
    
    double total_xp=0,total_xm=0,ave_xp=0,ave_xm=0;
    double total_yp=0,total_ym=0,ave_yp=0,ave_ym=0;
    
   

    // 幅
    int W = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    // 高さ
    int H = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    // 動画のfps
    double fps = capture.get(cv::CAP_PROP_FPS);
    int count = capture.get(cv::CAP_PROP_FRAME_COUNT);
    
    
    //卓球台の座標0〜３を格納する変数
    int table_w1;
    int table_h1;
    int table_w2;
    int table_h2;
    

    

    
//    printf("幅:%d\n",W);
//    printf("高さ:%d\n",H);
//    printf("fps:%f\n",fps);
//    printf("総フレーム数:%f",count);

//    保存用変数


    
//    格納用変数
    cv::Mat frameImage;
    cv::Mat backImage(cv::Size(W,H),CV_8UC3),mask_absdiff;
    cv::Mat contourImage;
    cv::Mat grayImage;
    cv::Mat edgeImage;
    cv::Mat frameImage2;
    cv::Mat edge_line,edge_circle;
    cv::Mat img_and,opticalImage;
    cv::Mat mask_white;
    cv::Mat move_image;
    
    
    cv::Mat frameImage3,gray2,binray2;
    
    cv::Mat Mask_rcnnImage,Mask_rcnnImage_bin,sourceImage_mask;
    

    cv::Mat Mask_rcnnImage_inv;
    
    cv::Mat frameImage_mask;

    cv::Mat testImage,testImage2,testImage3;

    cv::Mat channels_img;
    cv::Mat result_img;
    cv::Mat projection_img,translateImage;
    cv::Mat projection_Coordinate;
    cv::Mat hyoukaImage;
    cv::Mat beforeImage;
    
    cv::Mat persMat;
    
    cv::Point2f center;
    cv::Point2f center2;
    cv::Point2f center3;
    cv::Point2f optical;
    cv::Point2f optical2;
    cv::Point2f projection;
    
    

    
//    //確認用
//    cv::Mat subImage(cv::Size(W,H),CV_8UC3);
    cv::Mat gray,conner;
    
    //    グラフ追加
    /*------ データファイル作成 ---------- */
    const char *data_file;
    FILE   *gp,*data;
    data_file="out.dat";
    data = fopen(data_file,"w");
    int  j=1;
    int  com=0;
    double x;
//    int x2=3;
    int a[count];
    
    //ファイル表示
    FILE *fi;
    double frame,move;
    double x1,x2;
    
    
    static int name_count1 = 0;
    static int name_count2 = 0;

    double distance = 0;

    
    capture >> result_img;
    result_img.copyTo(projection_img);
    
    cv::Point2f original[4];
    
    original[0] = cv::Point2f(0,0);
    original[1] = cv::Point2f(projection_img.cols,0);
    original[2] = cv::Point2f(projection_img.cols,projection_img.rows);
    original[3] = cv::Point2f(0,projection_img.rows);
    



    while(1){
        
       
        
        int key = cv::waitKey(300);
        if (key=='q' || key=='Q'){
            fclose(data);

            break;
        }
        
        
//        if(4 > count_project){
//            cv::namedWindow("projection_img");
//            cv::moveWindow("projection_img", W, 0);
//            cv::imshow("projection_img",projection_img);
//            cv::setMouseCallback("projection_img", mouse_callback);
//            cv::circle(projection_img,mouse_click,1,cv::Scalar(0,255,0),3);
//
//
//            continue;
//        }
        
        capture >> frameImage;
        frameImage.copyTo(testImage);
        frameImage.copyTo(move_image);

        frameImage.copyTo(hyoukaImage);

        cv::Mat drawing = cv::Mat::zeros(H, W, CV_8UC3);



        if (frameImage.data==NULL)
        {
            fclose(data);

            break;
        }
        
        //射影変換
//        cv::Mat persMat = cv::getPerspectiveTransform(translate, original); //行列生成
//
//        //行列要素表示(確認用)
//         printf("%f %f %f\n", persMat.at<double>(0,0), persMat.at<double>(0,1), persMat.at<double>(0,2));
//         printf("%f %f %f\n", persMat.at<double>(1,0), persMat.at<double>(1,1), persMat.at<double>(1,2));
//         printf("%f %f %f\n", persMat.at<double>(2,0), persMat.at<double>(2,1), persMat.at<double>(2,2));
//
//        if(translateImage.data == NULL){
//            persMat = cv::getPerspectiveTransform(translate, original); //行列生成
            
            //行列要素表示(確認用)
//             printf("%f %f %f\n", persMat.at<double>(0,0), persMat.at<double>(0,1), persMat.at<double>(0,2));
//             printf("%f %f %f\n", persMat.at<double>(1,0), persMat.at<double>(1,1), persMat.at<double>(1,2));
//             printf("%f %f %f\n", persMat.at<double>(2,0), persMat.at<double>(2,1), persMat.at<double>(2,2));
            
//
//            cv::warpPerspective(projection_img, translateImage, persMat, projection_img.size(),
//            cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
//        }

//        cv::warpPerspective(testImage, testImage2, persMat, projection_img.size(),
//        cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
        
        
//        if (translate[0].x < translate[3].x)
//        {
//            table_w1 = translate[0].x;
//        }else if(translate[0].x > translate[3].x)
//        {
//            table_w1 = translate[3].x;
//        }
//
//        if (translate[1].x < translate[2].x)
//        {
//            table_w2 = translate[2].x;
//        }else if(translate[1].x > translate[2].x)
//        {
//            table_w2 = translate[1].x;
//        }
//
//        table_h1 = translate[0].y;
//        table_h2 = translate[3].y;
        
        
        
        //白検出
        mask_white = white_mask(frameImage);
        
        mask_absdiff = absdiff_mask(frameImage,backImage,W,H);
        backImage = frameImage.clone();

        //"frameImage"のコピーを"contourImage"に出力
        frameImage.copyTo(contourImage);
        frameImage.copyTo(conner);

        


        
        //白検出画像と領域抽出画像のAND画像img_andを出力
        cv::bitwise_and(mask_white, mask_absdiff, img_and);

        



        cv::cvtColor(conner, gray, cv::COLOR_BGR2GRAY); //グレースケールに変換
        cv::threshold(gray, gray, 50, 255, cv::THRESH_BINARY);



        
        
        //"binImage"から領域輪郭検出・ラベリング
        std::vector< std::vector<cv::Point> > contours; //領域輪郭群の格納用

        //全領域を検出して各領域輪郭を"contours"に格納
        cv::findContours(img_and, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
        //輪郭を順次呼び出し・表示
        //i 番目の領域のj 番目の頂点：contours[i][j]
        for (int i=0; i<contours.size(); i++) { //検出された領域数だけループ
            
            double length = cv::arcLength(contours[i], true); //領域"contous[i]"の輪郭の長さ
            double area = cv::contourArea(contours[i]); //領域"contous[i]"の面積
            double Circularity = (4*M_PI*area)/(length*length);
            
            double center_x = (contours[i][(contours.size()-1)/2].x + contours[i][0].x)/2;
            double center_y = (contours[i][(contours.size()-1)/2].y + contours[i][0].y)/2;

        
            center = cv::Point2f(center_x,center_y);
//            if((table_w1 < center.x && center.x < table_w2) && (table_h1 < center.y && center_y < table_h2))
//            {
//            cv::circle(contourImage,center,1,cv::Scalar(0,255,0),3);
            cv::rectangle(contourImage,cv::Point(center.x-5 , center.y-5),cv::Point(center.x+5,center.y+5),cv::Scalar(0,0,200), 1, 4);
//            cv::circle(contourImage,center2,1,cv::Scalar(0,0,255),3);
//            }


            cv::drawContours(drawing, contours, i, cv::Scalar(0,255,0), 2, 8);
        }
        
        
        //nとn-1番目のフレームの差を出して移動量を出す
        optical = center - center2;
        
        
       
        if(300 < optical.x || -300 > optical.x)
        {
            optical.x = 0;
        }
        a[j] = optical.x;
        
        printf("%d optical.y:%f, optical2.y:%f\n",j,optical.y,optical2.y );

        
//        fprintf(data,"%d\t%f\n", j, optical.x);
//
////        printf("%d番目のフレーム:%d\n",j,a[j]);
//        j++;
//        x軸
        distance = sqrt((center.x - center2.x) * (center.x - center2.x) + (center.y - center2.y) * (center.y - center2.y));
        
        fprintf(data,"%d\t%f\n", j, distance);

//        printf("%d番目のフレーム:%d\n",j,a[j]);
        j++;
//        printf("距離:%lf\n",distance);
//        if (distance < 45){
            if (optical.x > 0){
                cv::line(move_image, center2, cv::Point(center2.x + optical2.x,center2.y + optical2.y), cv::Scalar(0, 0, 255), 1, 8); //直線描画
               
                

            }else if(optical.x < 0){
//                cv::line(contourImage, center, center2, cv::Scalar(0, 255, 0), 1, 8); //直線描画
                cv::line(move_image, center2, cv::Point(center2.x + optical2.x,center2.y + optical2.y), cv::Scalar(0, 255, 0), 1, 8); //直線描画
            }
        
        //バウンド検出
        if (optical2.y >= 0  ){
            if( optical.y < 0){
                
//                cv::line(hyoukaImage, center, center2, cv::Scalar(0, 0, 0), 1, 8); //直線描画
//                cv::circle(hyoukaImage,center2,1,cv::Scalar(0,0,0),3);
//                if((translate[0].y <= center2.y && center2.y <= translate[3].y))
//                {
                    cv::rectangle(hyoukaImage,cv::Point(center2.x-5 , center2.y-5),cv::Point(center2.x+5,center2.y+5),cv::Scalar(0,0,255), 1, 4);
                    
//                }
//                    cv::line(result_img, center2, center3, cv::Scalar(0, 0, 0), 1, 8); //直線描画

//                 cv::circle(frameImage,center2,1,cv::Scalar(0,0,0),3);
                //射影変換後の座標
//                cv::Mat m1 = (cv::Mat_<double>(3,1) << center2.x,center2.y,1);
//                projection_Coordinate = persMat * m1;
//                projection = cv::Point2f(projection_Coordinate.at<double>(0,0),projection_Coordinate.at<double>(1,0));
//                cv::circle(translateImage,projection,1,cv::Scalar(0,0,255),3);


//                cv::rectangle(translateImage,cv::Point(projection.x-5 , projection.y-5),cv::Point(projection.x+5,projection.y+5),cv::Scalar(0,0,255), 1, 4);
                
//                cv::rectangle(testImage2,cv::Point(projection.x-5 , projection.y-5),cv::Point(projection.x+5,projection.y+5),cv::Scalar(0,0,200), 1, 4);

//                printf("フレーム数:%d x:%lf y:%lf \n",j,projection.x,projection.y);
                
//                cv::rectangle(testImage3,cv::Point(projection.x-5 , projection.y-5),cv::Point(projection.x+5,projection.y+5),cv::Scalar(0,0,255), 1, 4);

                


            }
        }
        
//        testImage2.copyTo(testImage3);
        
//        testImage3 = testImage2;
        
        
   

        //1フレーム前
        optical2 = optical;
        com ++;
        if(com>3){
            center3 = center2;
            com = 0;
        }

        

        
        //表示
//        cv::namedWindow("frameImage");
//        cv::moveWindow("frameImage", 0, 0);
//        cv::imshow("frameImage",frameImage);
//        cv::namedWindow("subImage");
//        cv::moveWindow("subImage", W, 0);
//        cv::imshow("subImage",subImage);
//        if (!(optical.x == 0))
//        {
//            cv::namedWindow("binray");
//            cv::moveWindow("binray", W, 0);
//            cv::imshow("binray",mask_absdiff);
//            cv::namedWindow("mask");
//            cv::moveWindow("mask",W*2, 0);
//            cv::imshow("mask",mask_white);
//            cv::namedWindow("img_and");
//            cv::moveWindow("img_and",W*3, 0);
//            cv::imshow("img_and",img_and);
        
//        cv::namedWindow("drawing");
//        cv::moveWindow("drawing",0, 0);
//        cv::imshow("drawing",drawing);
            cv::namedWindow("contourImage");
            cv::moveWindow("contourImage", 0, 0);
            cv::imshow("contourImage",contourImage);
        
        
//        cv::circle(result_img,translate,1,cv::Scalar(0,255,0),3);

        
//        cv::namedWindow("result_img");
//        cv::moveWindow("result_img", W*3, 0);
//        cv::imshow("result_img",result_img);
//
//        cv::namedWindow("translateImage");
//        cv::moveWindow("translateImage", W*2, 0);
//        cv::imshow("translateImage",translateImage);
//
//        cv::namedWindow("move_image");
//        cv::moveWindow("move_image", 0, H+20);
//        cv::imshow("move_image",move_image);
//
//        if(!(testImage3.data == NULL)){
//            cv::namedWindow("testImage3");
//            cv::moveWindow("testImage3", W, H+20);
//            cv::imshow("testImage3",testImage3);
//
//            std::ostringstream oss2;
//            oss2 << std::setfill( '0' ) << std::setw( 3 ) << name_count2++;
//            cv::imwrite( "hyouka-full/12/result_img2/output_12_" + oss2.str() + ".png", hyoukaImage );

//        }
        
//        if(!(hyoukaImage.data == NULL)){
            
            cv::imshow("hyoukaImage",hyoukaImage);
//            std::ostringstream oss2;
//            oss2 << std::setfill( '0' ) << std::setw( 3 ) << name_count2++;
//            cv::imwrite( "move/output_6_" + oss2.str() + ".png", move_image );
//        }
        
        
//        testImage3 = testImage2;
        testImage2.copyTo(testImage3);
        frameImage.copyTo(frameImage2);
        frameImage.copyTo(hyoukaImage);
//        hyoukaImage = frameImage;
        center2 = center;

        

        

//
//        std::ostringstream oss1;
//        oss1 << std::setfill( '0' ) << std::setw( 3 ) << name_count1++;
//        cv::imwrite( "hyouka-full/11/result_img/output_11_" + oss1.str() + ".png", result_img );
//

        
//        std::ostringstream oss2;
//        oss2 << std::setfill( '0' ) << std::setw( 3 ) << name_count2++;
//        cv::imwrite( "hyouka-full/12/frame/output_12_" + oss2.str() + ".png", frameImage );

        
//        writer << result_img;  // 画像 image を動画ファイルへ書き出す
//        writer_mask <<  result_img;

//        }
        
//        backImage = frameImage.clone();
        
        

//
       
    }
    


    //---Gnuplotを起動---
//    gp = popen("gnuplot -persist", "w");
//
//    //ラベル名
//    fprintf(gp, "set xlabel \"フレーム数\n");
//    fprintf(gp, "set ylabel \"移動距離\n");
//
//    //---座標の入力---
//    fprintf(gp, "set xrange [0:%d]\n",count);
//    fprintf(gp, "set xrange [0:%d]\n",count);
//
////    fprintf(gp, "set yrange [-300:300]\n");
//
//    //---Gnuplotのコマンドを実行---
//    fprintf(gp, "plot \"%s\" with lines linetype 1 title \"移動体の移動距離\"\n",data_file);
//
//    pclose(gp);
//
//
////    ファイル表示
////    読み込み
//
//    if((fi = fopen(data_file,"r"))==NULL)
//    {
//        //表示失敗
//        printf("ファイルがありません");
//    }else{
//
//        while(fscanf(fi,"%lf%lf",&frame,&move)==2)
//        {
//        //ファイルを出力
//            printf("%lf %lf\n",frame,move);
//            if(move > 0)
//            {
//                total_xp += move;
//                x1 ++;
//            }else if(move < 0)
//            {
//                total_xm += move;
//                x2++;
//            }
//
//        }
//        ave_xp = total_xp/x1;
//        ave_xm = total_xm/x2;
//        printf("---平均を表示する---\n");
//        printf("プラスの平均:%lf\n",ave_xp);
//      printf("マイナスの平均:%lf\n",ave_xm);
//        //ファイルを閉じる
//        fclose(fi);
//    }

    
    

    
   
    return 0;
}
