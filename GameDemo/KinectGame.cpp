#include <Kinect.h>
#include <windows.h>
#include <iostream>

#include "KinectGame.h"

static IKinectSensor* pSensor;
static IBodyFrameSource* pFrameSource;
static INT32 iBodyCount;
static IBody** aBody;
static IBodyFrame* pBodyFrame;
static IBodyFrameReader* pFrameReader;

bool KinectGame::KinectInit(void) {

	//Kinect传感器变量
	pSensor = nullptr;
	//判断Kinect传感器是否链接
	if (GetDefaultKinectSensor(&pSensor) != S_OK) {
		std::cout << "Get Sensor failed" << std::endl;
	}
	//判断是否打开传感器
	if (pSensor->Open() != S_OK) {
		std::cout << "Get Sensor failed" << std::endl;
	}

	//人体骨架信息来源变量
	pFrameSource = nullptr;
	if (pSensor->get_BodyFrameSource(&pFrameSource) != S_OK) {
		std::cerr << "Can't get body frame source" << std::endl;
	}

	//骨骼个数变量
	iBodyCount = 0;
	if (pFrameSource->get_BodyCount(&iBodyCount) != S_OK) {
		std::cerr << "Can't get body count" << std::endl;
	}

	//骨架数据
	aBody = new IBody * [iBodyCount];
	//骨骼数据初始化
	for (int i = 0; i < iBodyCount; ++i)
		aBody[i] = nullptr;

	//构造体Render
	std::cout << "Try to get body frame reader" << std::endl;
	pFrameReader = nullptr;
	if (pFrameSource->OpenReader(&pFrameReader) != S_OK) {
		std::cerr << "Can't get body frame reader" << std::endl;
	}

	pFrameSource->Release();
	pFrameSource = nullptr;

	return true;
}

bool KinectGame::KinectUpdate(void) {
	//骨骼数据变量
	IBodyFrame* pBodyFrame = nullptr;
	if (pFrameReader->AcquireLatestFrame(&pBodyFrame) == S_OK) {

		//得到骨骼数据
		if (pBodyFrame->GetAndRefreshBodyData(iBodyCount, aBody) == S_OK) {

			int iTrackedBodyCount = 0;

			//遍历相机得到的全部骨骼个数
			for (int i = 0; i < iBodyCount; ++i) {

				IBody* pBody = aBody[i];

				BOOLEAN bTracked = false;

				//判断是否取得了骨骼数组 返回布尔值
				if ((pBody->get_IsTracked(&bTracked) == S_OK) && bTracked) {

					++iTrackedBodyCount;
					std::cout << "User " << i << " is under tracking" << std::endl;

					//节点构造体
					Joint bodyJoints[JointType::JointType_Count];
					if (pBody->GetJoints(JointType::JointType_Count, bodyJoints) != S_OK) {
						std::cerr << "Get joints fail" << std::endl;
					}

					//节点姿态构造体
					JointOrientation jointOrientation[JointType::JointType_Count];
					if (pBody->GetJointOrientations(JointType::JointType_Count, jointOrientation) != S_OK) {
						std::cerr << "Get joints fail" << std::endl;
					}

					//循环取值
					for (int j = 0; j < JointType::JointType_Count; j++) {

						printf("%d/%d\n", j, JointType_Count);
						printf("position x:%f y:%f z:%f\n",
							bodyJoints[j].Position.X,
							bodyJoints[j].Position.Y,
							bodyJoints[j].Position.Z);
						printf("oraientation w:%f x:%f y:%f z:%f\n",
							jointOrientation[j].Orientation.w,
							jointOrientation[j].Orientation.x,
							jointOrientation[j].Orientation.y,
							jointOrientation[j].Orientation.z);
					}

					//骨骼节点类型设定
					JointType headJointType = JointType::JointType_Head;	//头
					//JointType	//
					JointType handRightJointType = JointType::JointType_HandRight;

					//关节POS
					const Joint& handRightJointPos = bodyJoints[handRightJointType];
					//关节姿态
					const JointOrientation& handRightJointOri = jointOrientation[handRightJointType];

					std::cout << " > Right Hand is ";
					if (handRightJointPos.TrackingState == TrackingState_NotTracked) {
						std::cout << "not tracked" << std::endl;
					}
					else {
						if (handRightJointPos.TrackingState == TrackingState_Inferred) {
							std::cout << "inferred ";
						}
						else if (handRightJointPos.TrackingState == TrackingState_Tracked) {
							std::cout << "tracked ";
						}

						std::cout << "at " << handRightJointPos.Position.X << ",\n\t orientation: " << handRightJointOri.Orientation.w << std::endl;
					}
				}
			}
			//判断当前相机前人数
			if (iTrackedBodyCount > 0)
				std::cout << "Total " << iTrackedBodyCount << " bodies in this time\n" << std::endl;
		}
		else {
			std::cerr << "Can't read body data" << std::endl;
		}
		pBodyFrame->Release();
	}

	return true;
}

void KinectGame::KinectRelease() {

	delete[] aBody;
	pFrameReader->Release();
	pFrameReader = nullptr;
	pSensor->Close();
	pSensor->Release();
	pSensor = nullptr;
}