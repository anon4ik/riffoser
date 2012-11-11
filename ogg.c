#include <riffoser_r.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>

// based on http://svn.xiph.org/trunk/vorbis/examples/encoder_example.c
int riffoser_ogg_savetofile(struct riffoser_io_struct *io) {
#define USE_OGGVORBIS 1
#ifndef USE_OGGVORBIS
	printf("Error: riffoser was compiled without ogg support (recompile with --with-ogg)\n");
	return (EXIT_FAILURE);
#else
	FILE *fp;
	ogg_stream_state os;
	ogg_page         og;
	ogg_packet       op;
	vorbis_info      vi;
	vorbis_comment   vc;
	vorbis_dsp_state vd;
	vorbis_block     vb;
	int eos=0,ret;
	int i, ii;
	unsigned int bytes=io->srcsize/io->channels;
	
	fp=fopen(io->filename,"wb");
	
	vorbis_info_init(&vi);


	ret = ( vorbis_encode_setup_managed(&vi,io->channels,io->samplerate,-1,io->kbps*1000,-1) || vorbis_encode_ctl(&vi,OV_ECTL_RATEMANAGE2_SET,NULL) || vorbis_encode_setup_init(&vi));

	if(ret) {
		printf("Error initializing ogg codec!\n");
		return (EXIT_FAILURE);
	}

	vorbis_comment_init(&vc);
	vorbis_comment_add_tag(&vc,"ENCODER",RIFFOSER_VERSION);
	
	vorbis_analysis_init(&vd,&vi);
	vorbis_block_init(&vd,&vb);
	
	srand(time(NULL));
	ogg_stream_init(&os,rand());
	
	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
		ogg_stream_packetin(&os,&header); /* automatically placed in its own page */
		ogg_stream_packetin(&os,&header_comm);
		ogg_stream_packetin(&os,&header_code);

		while(!eos){
			int result=ogg_stream_flush(&os,&og);
			if(result==0)break;
			fwrite(og.header,1,og.header_len,fp);
			fwrite(og.body,1,og.body_len,fp);
		}

	}
	
	float **buffer=vorbis_analysis_buffer(&vd,bytes);
	
	for (ii=0;ii<io->channels;ii++) {
		for (i=0;i<bytes;i++) {
			buffer[ii][i]=(float)((((io_src_t *)io->src)[i*io->channels+ii])*1.9-1);
		}
	}
	
	vorbis_analysis_wrote(&vd,bytes);
	
	while(vorbis_analysis_blockout(&vd,&vb)==1){

		vorbis_analysis(&vb,NULL);
		vorbis_bitrate_addblock(&vb);

		while(vorbis_bitrate_flushpacket(&vd,&op)){

			ogg_stream_packetin(&os,&op);

			while(!eos){
				int result=ogg_stream_pageout(&os,&og);
				if(result==0)
					break;
				fwrite(og.header,1,og.header_len,fp);
				fwrite(og.body,1,og.body_len,fp);

				if(ogg_page_eos(&og))eos=1;
			}
		}
	}
	
	vorbis_analysis_wrote(&vd,0);

	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);

	
	fclose(fp);
	
	return (EXIT_SUCCESS);
#endif
}

#define USE_OGGVORBIS 1

int riffoser_ogg_loadfromfile(struct riffoser_io_struct *io) {
#ifndef USE_OGGVORBIS
	printf("Error: riffoser was compiled without ogg support (recompile with --with-ogg)\n");
	return (EXIT_FAILURE);
#else
	ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page         og; /* one Ogg bitstream page. Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_comment   vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */	
	char *buffer;
	int  bytes;
	io_src_t convbuffer[4096];
	int convsize=4096;
	unsigned long len;
	FILE *fp;
	fp=fopen(io->filename,"rb");
	io_src_t val;

	ogg_sync_init(&oy); /* Now we can read pages */
	while(1){ /* we repeat if the bitstream is chained */
		int eos=0;
		int i;
		
		buffer=ogg_sync_buffer(&oy,4096);
		bytes=fread(buffer,1,4096,fp);
		ogg_sync_wrote(&oy,bytes);

		/* Get the first page. */
		if(ogg_sync_pageout(&oy,&og)!=1){
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
				int result=ogg_sync_pageout(&oy,&og);
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
			buffer=ogg_sync_buffer(&oy,4096);
			bytes=fread(buffer,1,4096,fp);
			if(bytes==0 && i<2){
				printf("Error: End of file before finding all Vorbis headers!\n");
				return (EXIT_FAILURE);
			}
			ogg_sync_wrote(&oy,bytes);
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
		
		convsize=4096/vi.channels;

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
					int result=ogg_sync_pageout(&oy,&og);
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
								int bout=(samples<convsize?samples:convsize);
                  
//								int min=0;
//								int max=0;
								for (i=0;i<bout;i++) {
									for (j=0;j<vi.channels;j++) {
										val=(io_src_t)(((float *)pcm[j])[i])/1.9+0.5; // magic
										convbuffer[i*vi.channels+j]=val;
									}
								}
								len=vi.channels*bout;
								if (io->srcsize==0)
									io->src=malloc(sizeof(io_src_t)*len);
								else io->src=realloc(io->src,sizeof(io_src_t)*(io->srcsize+len));
//								printf("sizeof(io_src_t)=%u io->srcsize=%u len=%u copyoffs=%u bytestocopy=%u io->src=%u copyto=%u\n",sizeof(io_src_t),io->srcsize,len,sizeof(io_src_t)*io->srcsize,sizeof(io_src_t)*len,io->src,io->src+io->srcsize);
								memcpy(io->src+io->srcsize,&convbuffer,sizeof(io_src_t)*len);
								io->srcsize+=len;
								for (i=0;i<io->srcsize;i++)
									if (io->src[i]>1) {
										printf("%f >=1 \n",io->src[i]);
										exit(1);
									}
//								fwrite(convbuffer,2*vi.channels,bout,stdout);
								vorbis_synthesis_read(&vd,bout);
							}
						}
					}
					if(ogg_page_eos(&og))eos=1;
				}
			}
			if(!eos){
				buffer=ogg_sync_buffer(&oy,4096);
				bytes=fread(buffer,1,4096,fp);
				ogg_sync_wrote(&oy,bytes);
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
  /* OK, clean up the framer */
	ogg_sync_clear(&oy);
  
	fclose(fp);
	
//	printf("Done (%lu bytes read).\n",io->srcsize);
	return(EXIT_SUCCESS);
	
#endif
}
