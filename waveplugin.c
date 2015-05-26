/* waveplugin.c
 *
 * ALSA I/O plug-in for capturing playout to WAVE files.
 *
 * Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
 *
 *    $Id: waveplugin.c,v 1.2 2009/08/10 00:43:48 cvs Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License 
 *   version 2 as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>

#include <sndfile.h>

struct wave_info {
  snd_pcm_ioplug_t ext;
  char filename[256];
  SNDFILE *sndfile;
  snd_pcm_uframes_t buffersize;
  unsigned periods;
};


/*
 * PCM Callbacks
 */
static int wave_pcm_start(snd_pcm_ioplug_t *io)
{
  return 0;
}


static int wave_pcm_stop(snd_pcm_ioplug_t *io)
{
  return 0;
}


snd_pcm_sframes_t wave_pcm_pointer(snd_pcm_ioplug_t *io)
{
  struct wave_info *wave=(struct wave_info *)io->private_data;
  return sf_seek(wave->sndfile,0,SEEK_CUR);
}


snd_pcm_sframes_t wave_pcm_transfer(snd_pcm_ioplug_t *io,
				  const snd_pcm_channel_area_t *areas,
				  snd_pcm_uframes_t offset,
				  snd_pcm_uframes_t size)
{
  int n;
  struct wave_info *wave=(struct wave_info *)io->private_data;

  n=sf_write_short(wave->sndfile,areas->addr,areas->step/16*size);
  return n*16/areas->step;
}


static int wave_pcm_close(snd_pcm_ioplug_t *io)
{
  struct wave_info *wave=(struct wave_info *)io->private_data;
  sf_close(wave->sndfile);
  return 0;
}


static int wave_pcm_hw_params(snd_pcm_ioplug_t *io, snd_pcm_hw_params_t *params)
{
  unsigned value;
  snd_pcm_uframes_t frames;
  SF_INFO sf_info;
  struct wave_info *wave=(struct wave_info *)io->private_data;

  snd_pcm_hw_params_get_buffer_size(params,&frames);
  wave->buffersize=frames;

  snd_pcm_hw_params_get_periods(params,&value,0);
  wave->periods=value;

  memset(&sf_info,0,sizeof(sf_info));
  sf_info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_16|SF_ENDIAN_LITTLE;

  snd_pcm_hw_params_get_rate(params,&value,0);
  sf_info.samplerate=value;

  snd_pcm_hw_params_get_channels(params,&value);
  sf_info.channels=value;

  if(wave->ext.stream==SND_PCM_STREAM_PLAYBACK) {
    if((wave->sndfile=sf_open(wave->filename,SFM_WRITE,&sf_info))==NULL) {
      fprintf(stderr,"couldn't open %s\n",wave->filename);
      free(wave);
      return -EINVAL;
    }
  }
  else {
  }

  return 0;
}


static snd_pcm_ioplug_callback_t wave_callbacks={
  .start=wave_pcm_start,
  .stop=wave_pcm_stop,
  .pointer=wave_pcm_pointer,
  .transfer=wave_pcm_transfer,
  .close=wave_pcm_close,
  .hw_params=wave_pcm_hw_params};


SND_PCM_PLUGIN_DEFINE_FUNC(wave)
{
  struct wave_info *wave;
  snd_config_iterator_t it;
  snd_config_iterator_t next;
  char *str;
  char filename[256];
  int err;

  /*
   * Hardware Parameters
   */
  unsigned access_types[1]={SND_PCM_ACCESS_RW_INTERLEAVED};
  unsigned formats[1]={SND_PCM_FORMAT_S16_LE};
  unsigned channels[2]={1,2};

  /*
   * Read Configuration
   */
  snd_config_for_each(it,next,conf) {
    snd_config_t *n=snd_config_iterator_entry(it);
    const char *id;
    if(snd_config_get_id(n,&id)<0) {
      continue;
    }
    if(strcmp(id,"filename")==0) {
      if(snd_config_get_string(n,&str)==0) {
	strncpy(filename,str,256);
      }
    }
  }

  /*
   * Initialize the Plug-in
   */
  if((wave=calloc(1,sizeof(struct wave_info)))==NULL) {
    return -ENOMEM;
  }
  memset(wave,0,sizeof(wave));
  wave->ext.version=SND_PCM_IOPLUG_VERSION;
  wave->ext.name="PCM Wave Capture Plug-in";
  wave->ext.callback=&wave_callbacks;
  wave->ext.private_data=wave;
  if((err=snd_pcm_ioplug_create(&(wave->ext),name,stream,mode))<0) {
    return err;
  }
  if(wave->ext.stream!=SND_PCM_STREAM_PLAYBACK) {
    fprintf(stderr,"capture not supported\n");
    return -EINVAL;
  }
  *pcmp=wave->ext.pcm;
  strcpy(wave->filename,filename);

  /*
   * Define Hardware Parameters
   */
  snd_pcm_ioplug_set_param_list(&(wave->ext),SND_PCM_IOPLUG_HW_ACCESS,
				1,access_types);
  snd_pcm_ioplug_set_param_list(&(wave->ext),SND_PCM_IOPLUG_HW_FORMAT,
				1,formats);
  snd_pcm_ioplug_set_param_list(&(wave->ext),SND_PCM_IOPLUG_HW_CHANNELS,
				2,channels);
  snd_pcm_ioplug_set_param_minmax(&(wave->ext),SND_PCM_IOPLUG_HW_RATE,
				  4000,192000);

  return 0;
}

SND_PCM_PLUGIN_SYMBOL(wave);
