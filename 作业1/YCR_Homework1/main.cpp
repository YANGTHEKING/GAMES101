#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
                 0, 1, 0, -eye_pos[1], 
                 0, 0, 1, -eye_pos[2], 
                 0, 0, 0, 1;//ƽ�����λ�õ�ԭ��,����Ӧ����Ĭ�ϲ�����ת����Ƕ���

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)//ģ�ͱ任����
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f rotation;
    double fangle = rotation_angle / 180 * MY_PI;//�Ƕ�ת���ȣ����ڼ���

    rotation << cos(fangle), -sin(fangle), 0, 0,
                sin(fangle), cos(fangle), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;//ģ����ת������z�ᣩ

    model = rotation * model;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)//ͶӰ�任����
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    Eigen::Matrix4f proj, ortho;

    proj << zNear, 0, 0, 0,
            0, zNear, 0, 0,
            0, 0, zNear + zFar, -zNear * zFar,
            0, 0, 1, 0;//͸��ͶӰ����

    double w, h, z;
    h = zNear * tan(eye_fov / 2) * 2;
    w = h * aspect_ratio;
    z = zFar - zNear;

    ortho << 2 / w, 0, 0, 0,
             0, 2 / h, 0, 0,
             0, 0, 2 / z, -(zFar+zNear)/2,
             0, 0, 0, 0;//����ͶӰ������Ϊ�ڹ۲�ͶӰʱx0yƽ���ӽ�Ĭ�������ģ��������������ͶӰ�Ͳ���ƽ��x��y��
    projection = ortho * proj * projection;

    return projection;
}

Eigen::Matrix4f get_rotation(Vector3f axis, float angle) {//��������ת�����޵����˹��ת��ʽ��Ĭ�����ԭ�㣩
    double fangle = angle / 180 * MY_PI;
    Eigen::Matrix4f I, N, Rod;
    Eigen::Vector4f axi;
    Eigen::RowVector4f taxi;

    axi << axis.x(), axis.y(), axis.z(), 0;
    taxi << axis.x(), axis.y(), axis.z(), 0;

    I << 1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1;

    N << 0, -axis.z(), axis.y(), 0,
         axis.z(), 0, -axis.x(), 0,
         -axis.y(), axis.x(), 0, 0,
         0, 0, 0, 1;
    
    Rod = cos(fangle) * I + (1 - cos(fangle)) * axi * taxi + sin(fangle) * N;
    Rod(3, 3) = 1;
    return Rod;
}

int main(int argc, const char** argv)
{
    float angle = 0;//����Ƕ�
    bool command_line = false;//���������п��ر�־��Ĭ��Ϊ��
    std::string filename = "output.png";//�����ļ�����Ĭ��Ϊoutput.png"

    Eigen::Vector3f raxis(0, 0, 1);
    double rangle = 0, ra;

    if (argc >= 3) {//���յ��Ĳ�����������������⵽ͨ�������д������ʱ
        command_line = true;//�������п��ر�־Ϊ��
        angle = std::stof(argv[2]); //�������л�ȡ�ǶȲ���
        if (argc == 4) {//���յ��Ĳ���Ϊ�ĸ�����ô˵���������������ļ�������
            filename = std::string(argv[3]);//�������л�ȡ�ļ���
        }
    }

    rst::rasterizer r(700, 700);//�趨700*700���صĹ�դ���ӿ�
    Eigen::Vector3f eye_pos = { 0, 0, 5 };//�趨���λ��
    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };//�趨������λ��
    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };//�趨���������,���ڻ�ͼʱȷ����Ҫ���������㣬�����ʾ������������

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);//������ͼ�εĶ������ţ�������ҵֻ�漰һ��ͼ�Σ����Բ���

    int key = 0;//��������
    int frame_count = 0;//֡���

    if (command_line) {//��������п��ر�־Ϊ������һ��if������Ϊ��Ӧ�������д���Ĳ����������ʼ�ǶȺ��ļ�����

        r.clear(rst::Buffers::Color | rst::Buffers::Depth);//��ʼ��֡�������Ȼ��棨������ҵ������ҵֻ�漰һ��ͼ�Σ����Բ��ܣ����Բ��漰��ȣ����Բ��ܣ�

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));//���դ������MVP����
        r.set_rodrigues(get_rotation(raxis, rangle));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);//��ʼ��դ����ͼ
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imwrite(filename, image);

        return 0;
    }

    bool rflag = false;

    std::cout << "Please enter the axis and angle:" << std::endl;
    std::cin >> raxis.x() >> raxis.y() >> raxis.z() >> ra;//�����޵����˹��ת��ͽ�

    while (key != 27) {//ֻҪû�м�⵽����ESC��ѭ��(ESC��ASCII����27)

        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        if (rflag) //�������r�ˣ��Ϳ�ʼ�Ƹ�����������ת
            r.set_rodrigues(get_rotation(raxis, rangle));
        else
            r.set_rodrigues(get_rotation({ 0,0,1 }, 0));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);//�ȴ���������

        std::cout << "frame count: " << frame_count++ << '\n';//��ʾ��ǰ�ǵڼ�֡����

        if (key == 'a') {//����a����ʱ����ת10��
            angle += 10;
        }
        else if (key == 'd') {//����d��˳ʱ����ת10��
            angle -= 10;
        }
        else if (key == 'r') {//����r���Ƹ�����������ת
            rflag = true;
            rangle += ra;
        }
    }

    return 0;

}