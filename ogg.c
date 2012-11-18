#include <riffoser_r.h>

#ifdef USE_OGG
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
struct riffoser_io_struct_ogg_write {
	vorbis_info vi;
	vorbis_comment vc;
	vorbis_dsp_state vd;
	vorbis_block vb;
	ogg_packet op;
	ogg_page og;
	ogg_stream_state os;
};
struct riffoser_io_struct_ogg_read {
	ogg_sync_state oy;

	
	OggVorbis_File vf;
	int current_section;
};
#define io_ogg_write ((struct riffoser_io_struct_ogg_write *)io->extra)
#define io_ogg_read ((struct riffoser_io_struct_ogg_read *)io->extra)
#else
#define OGG_ERR\
	printf("Error: riffoser was compiled without ogg support (recompile with --with-ogg)\n");\
	return (EXIT_FAILURE);
#endif

int riffoser_ogg_write_start(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	ogg_page og;
	int eos=0,ret;

	io->extra=malloc(sizeof(struct riffoser_io_struct_ogg_write));

	io->fp=fopen(io->filename,"wb");

	vorbis_info_init(&io_ogg_write->vi);

	ret = ( vorbis_encode_setup_managed(&io_ogg_write->vi,io->channels,io->samplerate,-1,io->kbps*1000,-1) || vorbis_encode_ctl(&io_ogg_write->vi,OV_ECTL_RATEMANAGE2_SET,NULL) || vorbis_encode_setup_init(&io_ogg_write->vi));

	if(ret) {
		printf("Error initializing ogg codec!\n");
		return (EXIT_FAILURE);
	}

	vorbis_comment_init(&io_ogg_write->vc);
	vorbis_comment_add_tag(&io_ogg_write->vc,"ENCODER",RIFFOSER_VERSION);

	vorbis_analysis_init(&io_ogg_write->vd,&io_ogg_write->vi);
	vorbis_block_init(&io_ogg_write->vd,&io_ogg_write->vb);

	srand(time(NULL));
	ogg_stream_init(&io_ogg_write->os,rand());

	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&io_ogg_write->vd,&io_ogg_write->vc,&header,&header_comm,&header_code);
		ogg_stream_packetin(&io_ogg_write->os,&header); /* automatically placed in its own page */
		ogg_stream_packetin(&io_ogg_write->os,&header_comm);
		ogg_stream_packetin(&io_ogg_write->os,&header_code);

		while(!eos){
			int result=ogg_stream_flush(&io_ogg_write->os,&og);
			if(result==0)break;
			fwrite(og.header,1,og.header_len,io->fp);
			fwrite(og.body,1,og.body_len,io->fp);
		}

	}

	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_write_bytes(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	int i, ii;
	unsigned int bytes=io->srcsize/io->channels;
	float **buffer;

	buffer=vorbis_analysis_buffer(&io_ogg_write->vd,bytes);

	for (ii=0;ii<io->channels;ii++)
		for (i=0;i<bytes;i++)
			buffer[ii][i]=(float)((((io_src_t *)io->src)[i*io->channels+ii]*2-1));

	vorbis_analysis_wrote(&io_ogg_write->vd,bytes);

	int eos=0;

	while(vorbis_analysis_blockout(&io_ogg_write->vd,&io_ogg_write->vb)==1){

		vorbis_analysis(&io_ogg_write->vb,NULL);
		vorbis_bitrate_addblock(&io_ogg_write->vb);

		while(vorbis_bitrate_flushpacket(&io_ogg_write->vd,&io_ogg_write->op)){

			ogg_stream_packetin(&io_ogg_write->os,&io_ogg_write->op);

			while(!eos){
				int result=ogg_stream_pageout(&io_ogg_write->os,&io_ogg_write->og);
				if(result==0)
					break;
				fwrite(io_ogg_write->og.header,1,io_ogg_write->og.header_len,io->fp);
				fwrite(io_ogg_write->og.body,1,io_ogg_write->og.body_len,io->fp);

				if(ogg_page_eos(&io_ogg_write->og))eos=1;
			}
		}
	}

	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_write_end(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	vorbis_analysis_wrote(&io_ogg_write->vd,0);

	ogg_stream_clear(&io_ogg_write->os);
	vorbis_block_clear(&io_ogg_write->vb);
	vorbis_dsp_clear(&io_ogg_write->vd);
	vorbis_comment_clear(&io_ogg_write->vc);
	vorbis_info_clear(&io_ogg_write->vi);
	fclose(io->fp);

	free(io->extra);

	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_read_start(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	vorbis_info *vi;

	io->extra=malloc(sizeof(struct riffoser_io_struct_ogg_read));

	io->fp=fopen(io->filename,"rb");

	if(ov_open_callbacks(io->fp, &io_ogg_read->vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
		fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
		return (EXIT_FAILURE);
	}

	vi=ov_info(&io_ogg_read->vf,-1);

	io->channels=vi->channels;
	io->kbps=vi->bitrate_nominal/1000;
	io->samplerate=vi->rate;
	io->datalen=ov_time_total(&io_ogg_read->vf,-1)*io->samplerate*io->channels;

	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_read_bytes(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	int enough=0;
	unsigned long i,ofs=0;

	unsigned short *buf;

	buf=malloc(io->srcsize*sizeof(unsigned short));

	while(!enough){
		
		long ret=ov_read(&io_ogg_read->vf,(char *)buf,(io->srcsize-ofs)*sizeof(unsigned short),0,sizeof(unsigned short),0,&io_ogg_read->current_section);
		ret/=sizeof(unsigned short);
		if (ret+ofs>io->srcsize) {
			printf("while reading %lu bytes:\n",(io->srcsize-ofs));
			printf("warning: excess bytes from ov_read\n");
			printf("read %lu bytes (ofs=%lu) ; %lu < %lu\n",ret,ofs);
		}
		if ((ret == 0)||(ret+ofs>io->srcsize)) {
		      /* EOF */
			enough=1;
		} else if (ret < 0) {
			printf("Error decoding file\n");
		} else {
			for (i=0;i<ret;i++) {
				io->src[i+ofs]=(io_src_t)(buf[i])/32768;
			}
			ofs+=ret;
			if (ofs>=io->srcsize)
				enough=1;
		}
	}

	free(buf);

	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_read_end(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	fclose(io->fp);

	free(io->extra);

	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_read_reset(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	riffoser_ogg_read_end(io);
	riffoser_ogg_read_start(io);
	return (EXIT_SUCCESS);
#endif
}

int riffoser_ogg_read_skip(struct riffoser_io_struct *io,unsigned long bytes) {
#ifndef USE_OGG
	OGG_ERR
#else
	return (EXIT_SUCCESS);
#endif
}

int _riffoser_ogg_read_start(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	OggVorbis_File vf;

	printf("start\n");

	io->extra=malloc(sizeof(struct riffoser_io_struct_ogg_read));

	io->fp=fopen(io->filename,"rb");

	if(ov_open_callbacks(io->fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
		fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
		exit(1);
	}

	ogg_sync_init(&io_ogg_read->oy);


	return (EXIT_SUCCESS);
#endif
}

int _riffoser_ogg_read_bytes(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page         og; /* one Ogg bitstream page. Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_comment   vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */
	char *buffer;
	int  bytes;
	unsigned long ofs=0;
	io_src_t val;
	unsigned char enough=0;
	unsigned int convsize;

//	printf("bytes %lu\n",io->srcsize);

	while(!enough){ /* we repeat if the bitstream is chained */
		int eos=0;
		int i;

		buffer=ogg_sync_buffer(&io_ogg_read->oy,4096);
		bytes=fread(buffer,1,4096,io->fp);
		ogg_sync_wrote(&io_ogg_read->oy,bytes);

		/* Get the first page. */
		if(ogg_sync_pageout(&io_ogg_read->oy,&og)!=1){
			/* have we simply run out of data?  If so, we're done. */
			if(bytes<4096)break;
			/* error case.  Must not be Vorbis data */
			printf("Error: Input does not appear to be an Ogg bitstream.\n");
			return (EXIT_FAILURE);
		}

		/* Get the serial number and set up the rest of decode. */
		/* serialno first; use it to set up a logical stream */
		ogg_stream_init(&os,ogg_page_serialno(&og));

		vorbis_info_init(&vi);
		vorbis_comment_init(&vc);
		if(ogg_stream_pagein(&os,&og)<0){
      /* error; stream version mismatch perhaps */
			printf("Error: Error reading first page of Ogg bitstream data.\n");
			return (EXIT_FAILURE);
		}

		if(ogg_stream_packetout(&os,&op)!=1){
			/* no page? must not be vorbis */
			printf("Error: Error reading initial header packet.\n");
			return (EXIT_FAILURE);
		}

		if(vorbis_synthesis_headerin(&vi,&vc,&op)<0){
		/* error case; not a vorbis header */
			printf("Error: This Ogg bitstream does not contain Vorbis audio data.\n");
			return (EXIT_FAILURE);
		}

		i=0;
		while(i<2){
			while(i<2){
				int result=ogg_sync_pageout(&io_ogg_read->oy,&og);
				if(result==0)break; /* Need more data */
        /* Don't complain about missing or corrupt data yet. We'll
           catch it at the packet output phase */
				if(result==1){
					ogg_stream_pagein(&os,&og); /* we can ignore any errors here
                                         as they'll also become apparent
                                         at packetout */
					while(i<2){
						result=ogg_stream_packetout(&os,&op);

						if(result==0)break;
						if(result<0){
              /* Uh oh; data at some point was corrupted or missing!
                 We can't tolerate that in a header.  Die. */
							printf("Error: Corrupt secondary header.  Exiting.\n");
							return (EXIT_FAILURE);
						}
						result=vorbis_synthesis_headerin(&vi,&vc,&op);
						if(result<0){
							printf("Error: Corrupt secondary header.  Exiting.\n");
							return (EXIT_FAILURE);
						}
						i++;
					}
				}
			}
      /* no harm in not checking before adding more */
			buffer=ogg_sync_buffer(&io_ogg_read->oy,4096);
			bytes=fread(buffer,1,4096,io->fp);

			if(bytes==0 && i<2){
				printf("Error: End of file before finding all Vorbis headers!\n");
				return (EXIT_FAILURE);
			}
			ogg_sync_wrote(&io_ogg_read->oy,bytes);
		}

    /* Throw the comments plus a few lines about the bitstream we're
       decoding */
		{
			char **ptr=vc.user_comments;
			while(*ptr){
//				printf("%s\n",*ptr);
				++ptr;
			}
			io->channels=vi.channels;
			io->samplerate=vi.rate;
//			printf("\nBitstream is %d channel, %ldHz\n",vi.channels,vi.rate);
//			printf("Encoded by: %s\n\n",vc.vendor);
		}

		convsize=io->srcsize/vi.channels;

    /* OK, got and parsed all three headers. Initialize the Vorbis
       packet->PCM decoder. */
		if(vorbis_synthesis_init(&vd,&vi)==0){ /* central decode state */
			vorbis_block_init(&vd,&vb);          /* local state for most of the decode
                                              so multiple block decodes can
                                              proceed in parallel. We could init
                                              multiple vorbis_block structures
                                              for vd here */


      /* The rest is just a straight decode loop until end of stream */
			while(!eos){
				while(!eos){
					int result=ogg_sync_pageout(&io_ogg_read->oy,&og);
					if(result==0)break; /* need more data */
					if(result<0){ /* missing or corrupt data at this page position */
						printf("Warning: Corrupt or missing data in bitstream; continuing...\n");
					}else{
						ogg_stream_pagein(&os,&og); /* can safely ignore errors at
                                           this point */
						while(1){
						result=ogg_stream_packetout(&os,&op);

						if(result==0)break; /* need more data */
						if(result<0){ /* missing or corrupt data at this page position */
                /* no reason to complain; already complained above */
						}else{
                /* we have a packet.  Decode it */
							float **pcm;
							int samples;

							if(vorbis_synthesis(&vb,&op)==0) /* test for success! */
								vorbis_synthesis_blockin(&vd,&vb);

							while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0){
								int j;
								int clipflag=0;
								int bout=(samples<convsize-ofs?samples:convsize-ofs);
//								printf("bout=%lu samples=%lu convsize=%lu ofs=%lu\n",bout,samples,convsize,ofs);
								//								int min=0;
//								int max=0;
								for (i=0;i<bout;i++) {
									for (j=0;j<vi.channels;j++) {
										val=(io_src_t)(((float *)pcm[j])[i])/1.9+0.5; // magic
//										printf("addr=%lu\n",i*vi.channels+ofs+j);
										if (i*vi.channels+ofs+j>=io->srcsize) {
											printf("OVERFLOW (%lu excess bytes)\n",(bout+ofs-1)*vi.channels+vi.channels-io->srcsize);
//											bout=i;
											exit(1);
										}
										io->src[i*vi.channels+ofs+j]=val;
									}
								}
								ofs+=bout*vi.channels;
								vorbis_synthesis_read(&vd,bout);
								
							}
						}
					}
					if(ogg_page_eos(&og))eos=1;
				}
			}
			if(!eos){
				buffer=ogg_sync_buffer(&io_ogg_read->oy,4096);
				bytes=fread(buffer,1,4096,io->fp);
				ogg_sync_wrote(&io_ogg_read->oy,bytes);
				if(bytes==0)eos=1;
			}
		}
	} else {
		printf("Error: Corrupt header during playback initialization.\n");
	}
    /* clean up this logical bitstream; before exit we see if we're
       followed by another [chained] */

	ogg_stream_clear(&os);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);  /* must be called last */
	}

//	printf("/bytes %lu\n",io->srcsize);

	return (EXIT_SUCCESS);
#endif
}

int _riffoser_ogg_read_end(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	printf("end\n");

	ogg_sync_clear(&io_ogg_read->oy);

	fclose(io->fp);

	free(io->extra);

	return (EXIT_SUCCESS);
#endif
}

int _riffoser_ogg_read_reset(struct riffoser_io_struct *io) {
#ifndef USE_OGG
	OGG_ERR
#else
	printf("reset\n");

	riffoser_ogg_read_end(io);
	riffoser_ogg_read_start(io);

#endif
}

int _riffoser_ogg_read_skip(struct riffoser_io_struct *io,unsigned long bytes) {
#ifndef USE_OGG
	OGG_ERR
#else
	printf("skip %li\n",bytes);

#endif
}
