#include <riffoser_r.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>

// based on http://svn.xiph.org/trunk/vorbis/examples/encoder_example.c
int riffoser_ogg_savetofile(struct riffoser_io_struct *io) {
#ifndef USE_OGGVORBIS
	printf("Error: riffoser was compiled without ogg support (recompile with --with-libogg)\n");
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
	unsigned int bytes=io->srcsize/io->bytespersample/io->channels;
	
	fp=fopen(io->filename,"wb");
	
	vorbis_info_init(&vi);

	ret = ( vorbis_encode_setup_managed(&vi,io->channels,io->samplerate,-1,io->kbps*1000,-1) || vorbis_encode_ctl(&vi,OV_ECTL_RATEMANAGE2_SET,NULL) || vorbis_encode_setup_init(&vi));

	if(ret) {
		printf("Error initializing ogg codec!\n");
		return (EXIT_FAILURE);
	}
	
	vorbis_comment_init(&vc);
	vorbis_comment_add_tag(&vc,"ENCODER","encoder_example.c");
	
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
			if (io->bytespersample==1)
				buffer[ii][i]=(float)((((unsigned char *)io->src)[i*io->channels+ii]))/128;
			else if (io->bytespersample==2)
				buffer[ii][i]=(float)((((short *)io->src)[i*io->channels+ii]))/32768;
			else if (io->bytespersample==4)
				buffer[ii][i]=(float)((((int *)io->src)[i*io->channels+ii]))/2147483648;
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

int riffoser_ogg_loadfromfile(struct riffoser_io_struct *io) {
#ifndef USE_OGGVORBIS
	printf("Error: riffoser was compiled without ogg support (recompile with --with-libogg)\n");
	return (EXIT_FAILURE);
#else
	printf("ogg loadfromfile not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}
