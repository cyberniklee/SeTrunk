/*
 * MapGenerator.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: root
 */

#include "MapGenerator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h>

#define SAVE_VIDEO_FRAME (1)
#define VIDEO_INPUT (1)
#define FS_WRITER (1)

#define CALC_Y(r, g, b) ((299 * r + 587 * g + 114 * b) / 1000)
#define CALC_U(r, g, b) ((-169 * r - 331 * g + 500 * b) / 1000 + 128)
#define CALC_V(r, g, b) ((500 * r - 419 * g - 81 * b) / 1000 + 128)

namespace NS_NaviCommon
{
  static int videoEos = 0;

  bool MapGenerator::lock(int fd)
  {
    if(flock(fd, LOCK_EX) < 0)
    {
      return false;
    }
    return true;
  }

  bool MapGenerator::unlock(int fd)
  {
    if(flock(fd, LOCK_UN) < 0)
    {
      return false;
    }
    return true;
  }

  /*
  bool MapGenerator::saveMapInPGM(std::vector< char > map_data,
                                  int height, int width,
                                  std::string pgm_file)
  {
    int pgm_fd = open(pgm_file.c_str(), O_RDWR | O_CREAT);
    if(pgm_fd < 0)
    {
      printf("Open file %s fail!", pgm_file.c_str());
      return false;
    }

    if(!lock(pgm_fd))
    {
      printf("Lock file %s fail!", pgm_file.c_str());
      return false;
    }

    int resize_width = width;
    int resize_height = height;
    int pix255 = 255;
    int pix000 = 000;
    int pix099 = 99;
    int pix205 = 205;

    while((resize_width % 4) != 0)
    {
      resize_width++;
    }

    while((resize_height % 4) != 0)
    {
      resize_height++;
    }

    char meta_line[128] =
      {0};
    sprintf(meta_line, "P5\n%d %d\n255\n", resize_width, resize_height);
    write(pgm_fd, meta_line, sizeof(meta_line));

    for(int y = 0; y < height; y++)
    {
      for(int x = 0; x < width; x++)
      {
        int i = x + (height - y - 1) * width;
        if(map_data[i] == 0)
        { //occ [0,0.1)
          write(pgm_fd, &pix255, 1);
        }
        else if(map_data[i] == +100)
        { //occ (0.65,1]
          write(pgm_fd, &pix000, 1);
        }
        else if(map_data[i] == 99)
        {
          write(pgm_fd, &pix099, 1);
        }
        else
        { //occ [0.1,0.65]
          write(pgm_fd, &pix205, 1);
        }
      }
      for(int k = 0; k < (resize_width - width); k++)
      {
        write(pgm_fd, &pix000, 1);
      }
    }
    for(int y = 0; y < resize_height - height; y++)
    {
      for(int x = 0; x < resize_width; x++)
      {
        write(pgm_fd, &pix000, 1);
      }
    }

    unlock(pgm_fd);

    close(pgm_fd);

    return true;

  }

  bool MapGenerator::readMapFromPGM(std::string pgm_file,
                                    std::vector< char >& map_data,
                                    int& height, int& width)
  {
    int pgm_fd = open(pgm_file.c_str(), O_RDWR);
    if(pgm_fd < 0)
    {
      printf("Open file %s fail!", pgm_file.c_str());
      return false;
    }

    if(!lock(pgm_fd))
    {
      printf("Lock file %s fail!", pgm_file.c_str());
      return false;
    }

    char line[70] =
      {0};
    int max_val = 0;
    bool binary;
    int num_read = 0;
    int test, temp;

    while(read(pgm_fd, line, sizeof(line)) == 0)
      continue;

    while(line[0] == '#' || line[0] == '\n' || read(pgm_fd, line, sizeof(line)) == 0)
    {
      continue;
    }

    if((line[0] == 'P') && (line[1] == '5'))
    {
      binary = true;
    }

    while(read(pgm_fd, line, sizeof(line)) == 0)
      continue;

    sscanf(line, "%d %d", &width, &height);

    printf("width:%d  height:%d", width, height);

    while(read(pgm_fd, line, sizeof(line)) == 0)
      continue;

    sscanf(line, "%d", &max_val);

    printf("max value: %d", max_val);

    if(width < 1 || height < 1 || max_val < 0 || max_val > 256)
    {
      printf("error picture!");
      return false;
    }
    else if(width > 2048 || height > 2048)
    {
      printf("map is too large!");
      return false;
    }

    int size = width * height;

    map_data.resize(size);

    if(binary)
    {
      for(int i = 0; i < size; i++)
      {
        int flag = read(pgm_fd, &map_data[i], 1);
        if(flag == 0 || flag == -1)
          break;
      }
    }
    else
    {
      printf("error format %d", binary);
      return false;
    }

    unlock(pgm_fd);

    close(pgm_fd);

    return true;

  }
  */

  bool MapGenerator::addPointInMap(std::vector< char >& map_data,
                                   int height, int width,
                                   int x, int y,
                                   GridPointValue point)
  {
    for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
      {
        int k = i * width + j;
        if (j == x && i == y)
        {
          map_data[k] = point;
          return true;
        }
      }
    }

    return false;
  }

  bool MapGenerator::addRobotPoseInMap(std::vector< char >& map_data,
                                       int map_height, int map_width,
                                       int robot_pose_x, int robot_pose_y)
  {
    return addPointInMap(map_data, map_height, map_width, robot_pose_x, robot_pose_y, PGM_ROBOT_POSE);
  }

  bool MapGenerator::addPathPointInMap(std::vector< char >& map_data,
                                       int map_height, int map_width,
                                       int path_pose_x, int path_pose_y)
  {
    return addPointInMap(map_data, map_height, map_width, path_pose_x, path_pose_y, PGM_PATH);
  }

  bool MapGenerator::addTargetInMap(std::vector< char >& map_data,
                                    int map_height, int map_width,
                                    int goal_pose_x, int goal_pose_y)
  {
    return addPointInMap(map_data, map_height, map_width, goal_pose_x, goal_pose_y, PGM_TARGET);
  }

  void MapGenerator::mapToYuv(std::vector< char >& pgm,
                              std::vector< char >& yuv,
                              int height, int width,
                              int& resize_height, int& resize_width)
  {
    int size = height * width;
    int yuv_size = ((size * 3) / 2);
    yuv.resize(yuv_size);

    std::vector< char > rgb;
    rgb.resize(size);

    resize_width = width;
    resize_height = height;
    while((resize_width % 4) != 0)
    {
      resize_width++;
    }

    while((resize_height % 4) != 0)
    {
      resize_height++;
    }

    int pos = 0;
    for(int y = 0; y < height; y++)
    {
      for(int x = 0; x < width; x++)
      {
        rgb[pos++] = pgm[pos];
      }
      for(int k = 0; k < (resize_width - width); k++)
      {
         rgb[pos++] = PGM_UNKNOWN_AREA;
      }
    }
    for(int y = 0; y < resize_height - height; y++)
    {
      for(int x = 0; x < resize_width; x++)
      {
        rgb[pos++] = PGM_UNKNOWN_AREA;
      }
    }

    char* it_y = &yuv[0];
    char* it_u = it_y + size;
    char* it_v = it_u + size / 4;

    int rgb_point;
    unsigned char y, u, v, r, g, b;

    //RGB[0,255]--->YUV[0,255]
    for(int j = 0; j < height; j++)
    {
      for(int i = 0; i < width; i++)
      {
        rgb_point = rgb[j * width + i];

        switch(rgb_point)
        {
          case PGM_UNKNOWN_AREA:
            //gray
            y = CALC_Y(206, 206, 206);
            u = CALC_U(206, 206, 206);
            v = CALC_V(206, 206, 206);
            break;
          case PGM_SCAN_EDGE:
            //black
            y = CALC_Y(0, 0, 0);
            u = CALC_U(0, 0, 0);
            v = CALC_V(0, 0, 0);
            break;
          case PGM_INFLATION:
            //deep gray
            y = CALC_Y(169, 169, 169);
            u = CALC_U(169, 169, 169);
            v = CALC_V(169, 169, 169);
            break;
          case PGM_KNOWN_AREA:
            //white
            y = CALC_Y(254, 254, 254);
            u = CALC_U(254, 254, 254);
            v = CALC_V(254, 254, 254);
            break;
          case PGM_ROBOT_POSE:
            //yellow
            y = CALC_Y(255, 255, 0);
            u = CALC_U(255, 255, 0);
            v = CALC_V(255, 255, 0);
            break;
          case PGM_TARGET:
            //blue
            y = CALC_Y(0, 0, 255);
            u = CALC_U(0, 0, 255);
            v = CALC_V(0, 0, 255);
            break;
          case PGM_PATH:
            //red
            y = CALC_Y(220, 20, 60);
            u = CALC_U(220, 20, 60);
            v = CALC_V(220, 20, 60);
            break;
          default:
            y = CALC_Y(206, 206, 206);
            u = CALC_U(206, 206, 206);
            v = CALC_V(206, 206, 206);
            break;
        }

        if(y > 255)
          y = 255;

        if(y < 0)
          y = 0;

        *(it_y++) = y;

        if((j % 2 == 0) && (i % 2 == 0))
        {
          if(u > 255)
            u = 255;

          if(u < 0)
            u = 0;

          *(it_u++) = u;

          if(v > 255)
            v = 255;

          if(v < 0)
            v = 0;

          *(it_v++) = v;
        }
      }
    }
  }

  void* MapGenerator::yuvInputThread(void* context)
  {
    DemoRecoderContext *p = (DemoRecoderContext*)context;

    int sizeY = p->videoConfig.nSrcHeight * p->videoConfig.nSrcWidth;

    struct ScMemOpsS* memops = NULL;

    VideoInputBuffer videoInputBuffer;

    if(p->videoConfig.bUsePhyBuf)
    {
      memops = MemAdapterGetOpsS();
      if(memops == NULL)
      {
        printf("MemAdapterGetOpsS failed!!\n\n");
        return NULL;
      }
      CdcMemOpen(memops);
      p->pAddrPhyY = (char*)CdcMemPalloc(memops, sizeY);
      p->pAddrPhyC = (char*)CdcMemPalloc(memops, sizeY / 2);

      videoInputBuffer.nID = 0;
      memcpy(p->pAddrPhyY, &p->yuvData[0], sizeY);
      memcpy(p->pAddrPhyC, &p->yuvData[sizeY], sizeY / 2);

      CdcMemFlushCache(memops, p->pAddrPhyY, sizeY);
      CdcMemFlushCache(memops, p->pAddrPhyC, sizeY / 2);

      videoInputBuffer.pAddrPhyY = (unsigned char*)CdcMemGetPhysicAddressCpu(
          memops, p->pAddrPhyY);
      videoInputBuffer.pAddrPhyC = (unsigned char*)CdcMemGetPhysicAddressCpu(
          memops, p->pAddrPhyC);
    }
    /*
    else
    {
      memset(&videoInputBuffer, 0, sizeof(VideoInputBuffer));
      videoInputBuffer.nLen = p->videoConfig.nSrcHeight * p->videoConfig.nSrcWidth * 3 / 2;
      videoInputBuffer.pData = (unsigned char*)malloc(videoInputBuffer.nLen);
    }
    */

    int size;
    long long videoPts = 0;

    int ret = -1;

    while(ret < 0)
    {
      videoInputBuffer.nPts = videoPts;
      p->bUsed = 1;

      ret = AwEncoderWriteYUVdata(p->mAwEncoder, &videoInputBuffer);

      usleep(10 * 1000);
    }
    usleep(29 * 1000);
    videoPts += 1000 / p->videoConfig.nFrameRate;

    videoEos = 1;
    if(p->pAddrPhyY)
    {
      CdcMemPfree(memops, p->pAddrPhyY);
    }

    if(p->pAddrPhyC)
    {
      CdcMemPfree(memops, p->pAddrPhyC);
    }
    if(memops)
    {
      CdcMemClose(memops);
      memops = NULL;
    }

    return NULL;
  }

  void* MapGenerator::muxerThread(void* context)
  {
    DemoRecoderContext *p = (DemoRecoderContext*)context;
    RecoderWriterT *rw = NULL;
#if FS_WRITER
    CdxFsCacheMemInfo fs_cache_mem;
#endif

    if(p->pUrl)
    {
      if((p->pStream = CdxWriterCreat()) == NULL)
      {
        printf("creat it failed!!!\n\n");
        return NULL;
      }
      rw = (RecoderWriterT*)p->pStream;
      rw->file_mode = FD_FILE_MODE;
      strcpy(rw->file_path, p->pUrl);
      RWOpen(p->pStream);

      p->pMuxer = CdxMuxerCreate(p->muxType, p->pStream);
      if(p->pMuxer == NULL)
      {
        printf("CdxMuxerCreate failed\n");
        return NULL;
      }
    }

    CdxMuxerMediaInfoT mediainfo;
    memset(&mediainfo, 0, sizeof(CdxMuxerMediaInfoT));

#if VIDEO_INPUT
    mediainfo.videoNum = 1;
#endif
    if(p->videoConfig.nType == VIDEO_ENCODE_H264)
      mediainfo.video.eCodeType = VENC_CODEC_H264;
    else if(p->videoConfig.nType == VIDEO_ENCODE_JPEG)
      mediainfo.video.eCodeType = VENC_CODEC_JPEG;
    else
    {
      printf("cannot support this video type");
      return NULL;
    }

    mediainfo.video.nWidth = p->videoConfig.nOutWidth;
    mediainfo.video.nHeight = p->videoConfig.nOutHeight;
    mediainfo.video.nFrameRate = p->videoConfig.nFrameRate;

    if(p->pMuxer)
    {
      CdxMuxerSetMediaInfo(p->pMuxer, &mediainfo);
#if FS_WRITER
      memset(&fs_cache_mem, 0, sizeof(CdxFsCacheMemInfo));

      fs_cache_mem.m_cache_size = 512 * 1024;
      fs_cache_mem.mp_cache = (cdx_int8*)malloc(fs_cache_mem.m_cache_size);
      if (fs_cache_mem.mp_cache == NULL)
      {
        printf("fs_cache_mem.mp_cache malloc failed\n");
        return NULL;
      }

      CdxMuxerControl(p->pMuxer, SET_CACHE_MEM, &fs_cache_mem);
      int fs_mode = FSWRITEMODE_CACHETHREAD;

      CdxMuxerControl(p->pMuxer, SET_FS_WRITE_MODE, &fs_mode);
#endif
    }

    if(p->extractDataLength > 0 && p->pMuxer)
    {

      if(p->pMuxer)
        CdxMuxerWriteExtraData(p->pMuxer, p->extractDataBuff,
                               p->extractDataLength, 0);
    }

    if(p->pMuxer)
    {
      CdxMuxerWriteHeader(p->pMuxer);
    }
#if VIDEO_INPUT
    while(videoEos == 0)
#endif
    {
      if(p->pMuxer)
      {
        if(p->hasPacket == 1)
        {
          if(CdxMuxerWritePacket(p->pMuxer, p->packet) < 0)
          {
            return NULL;
          }
          p->hasPacket == 0;
        }
        usleep(1 * 1000);
      }
    }

    if(p->pMuxer)
    {
      CdxMuxerWriteTrailer(p->pMuxer);
      CdxMuxerClose(p->pMuxer);
      p->pMuxer = NULL;
    }

    if(p->pStream)
    {
      RWClose(p->pStream);
      CdxWriterDestroy(p->pStream);
      p->pStream = NULL;
      rw = NULL;
    }

#if FS_WRITER
    if(fs_cache_mem.mp_cache)
    {
      free(fs_cache_mem.mp_cache);
      fs_cache_mem.mp_cache = NULL;
    }
#endif

    p->exitFlag = 1;
    return NULL;
  }

  int MapGenerator::onVideoDataEnc(void *context, CdxMuxerPacketT *buff)
  {
    DemoRecoderContext* pDemoRecoder = (DemoRecoderContext*)context;
    if(!buff)
      return 0;

    pDemoRecoder->packet = (CdxMuxerPacketT*)malloc(sizeof(CdxMuxerPacketT));
    pDemoRecoder->packet->buflen = buff->buflen;
    pDemoRecoder->packet->length = buff->length;
    pDemoRecoder->packet->buf = malloc(buff->buflen);
    memcpy(pDemoRecoder->packet->buf, buff->buf, pDemoRecoder->packet->buflen);
    pDemoRecoder->packet->pts = buff->pts;
    pDemoRecoder->packet->type = buff->type;
    pDemoRecoder->packet->streamIndex = buff->streamIndex;
    pDemoRecoder->packet->duration = buff->duration;

#if SAVE_VIDEO_FRAME
    if(pDemoRecoder->fpSaveVideoFrame)
    {
      fwrite(pDemoRecoder->packet->buf, 1, pDemoRecoder->packet->buflen,
             pDemoRecoder->fpSaveVideoFrame);
    }
#endif
    pDemoRecoder->hasPacket = 1;
    return 0;
  }

  void MapGenerator::notifyForAwEncorder(void* context, int msg, void* param)
  {
    DemoRecoderContext* pDemoRecoder = (DemoRecoderContext*)context;

    switch(msg)
    {
      case AWENCODER_VIDEO_ENCODER_NOTIFY_RETURN_BUFFER:
      {
        int id = *((int*)param);
        if(id == 0)
        {
          pDemoRecoder->bUsed = 0;
        }
        break;
      }
      default:
      {
        printf("warning: unknown callback from AwRecorder.\n");
        break;
      }
    }
  }

  bool MapGenerator::compressYuvToJpeg(std::vector< char >& yuv,
                                       int height, int width,
                                       std::string jpeg_file)
  {
    DemoRecoderContext demoRecoder;
    memset(&demoRecoder, 0, sizeof(DemoRecoderContext));

    videoEos = 0;

    EncDataCallBackOps mEncDataCallBackOps;
    mEncDataCallBackOps.onVideoDataEnc = onVideoDataEnc;
    mEncDataCallBackOps.onAudioDataEnc = NULL;

#if SAVE_VIDEO_FRAME
    demoRecoder.fpSaveVideoFrame = fopen(jpeg_file.c_str(), "wb");
    if(demoRecoder.fpSaveVideoFrame == NULL)
    {
      printf("open file video.jpeg failed, errno(%d)\n", errno);
      return false;
    }
#endif

    demoRecoder.muxType = CDX_MUXER_MOV;   //  mux type: 0:MP4 1:TS 3:AAC 4:MP3
    demoRecoder.hasPacket = 0;
    memset(&demoRecoder.videoConfig, 0, sizeof(VideoEncodeConfig));
    demoRecoder.videoConfig.nType = VIDEO_ENCODE_JPEG; //   video enc type: 0:H264 1:JPEG
    demoRecoder.videoConfig.nInputYuvFormat = VENC_PIXEL_YUV420P;
    demoRecoder.videoConfig.nFrameRate = 30;            //30
    demoRecoder.videoConfig.nOutHeight = height;
    demoRecoder.videoConfig.nOutWidth = width;
    demoRecoder.videoConfig.nSrcHeight = height;                           //288
    demoRecoder.videoConfig.nSrcWidth = width;                             //352
    demoRecoder.videoConfig.nBitRate = 3 * 1000 * 1000;
    demoRecoder.videoConfig.bUsePhyBuf = 1;
    demoRecoder.yuvData = yuv;
    demoRecoder.exitFlag = 0;       //init

    demoRecoder.mAwEncoder = AwEncoderCreate(&demoRecoder);
    if(demoRecoder.mAwEncoder == NULL)
    {
      printf("can not create AwRecorder, quit.\n");
      return false;
    }

    AwEncoderSetNotifyCallback(demoRecoder.mAwEncoder, notifyForAwEncorder, &(demoRecoder));

    AwEncoderInit(demoRecoder.mAwEncoder, &demoRecoder.videoConfig, NULL, &mEncDataCallBackOps);

    sprintf(demoRecoder.pUrl, "/tmp/gmap.mp4");

    AwEncoderStart(demoRecoder.mAwEncoder);

    AwEncoderGetExtradata(demoRecoder.mAwEncoder, &demoRecoder.extractDataBuff, &demoRecoder.extractDataLength);

#if SAVE_VIDEO_FRAME
    if(demoRecoder.fpSaveVideoFrame)
    {
      fwrite(demoRecoder.extractDataBuff, 1, demoRecoder.extractDataLength,
             demoRecoder.fpSaveVideoFrame);
    }
#endif
    pthread_create(&demoRecoder.videoDataThreadId, NULL, yuvInputThread,
                   &demoRecoder);

    pthread_create(&demoRecoder.muxerThreadId, NULL, muxerThread, &demoRecoder);

    while(demoRecoder.exitFlag == 0)
    {
      printf("wait MuxerThread finish!\n\n");
      usleep(1000 * 1000);
    }

    if(demoRecoder.muxerThreadId)
      pthread_join(demoRecoder.muxerThreadId, NULL);

#if VIDEO_INPUT
    if(demoRecoder.videoDataThreadId)
      pthread_join(demoRecoder.videoDataThreadId, NULL);
#endif
    if(demoRecoder.mAwEncoder != NULL)
    {
      AwEncoderStop(demoRecoder.mAwEncoder);
      AwEncoderDestory(demoRecoder.mAwEncoder);
      demoRecoder.mAwEncoder = NULL;
    }

#if SAVE_VIDEO_FRAME
    if(demoRecoder.fpSaveVideoFrame)
      fclose(demoRecoder.fpSaveVideoFrame);
#endif
    return true;
  }

} /* namespace NS_NaviCommon */
