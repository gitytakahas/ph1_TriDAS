/*
   FileName : 		GLUE.C

   Content : 		Entry points for users low-level accesses to the
   			FEC device driver - exports of and comments about
			functions is performed in file includes/glue.h

   Used in : 		Device Driver interface library (libfec_glue.a/o/so)
   			Client Programs

   Programmer : 	Laurent GROSS

   Version : 		Unified-6.0
   
   Date of last modification : 03/05/2005

   Support : 		mail to : fec-support@ires.in2p3.fr
*/


/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>



#include "datatypes.h"
#include "defines.h"
#include "fec_ioctl.h"
#include "pci_ioctl.h"
#include "plx_ioctl.h"
#include "dd_perror.h"
#include "dderrors.h"



DD_TYPE_ERROR glue_fec_get_fifo_item_size(int, int *);

//#define DEBUG_R16_S32
//#define DEBUG_G32_W16


/* used for GET methods on fifos */
/*
static int glb_fifotra_stat_memory_status=0;
static DD_FEC_FIFO_DATA_16 glb_fifotra_stat_memory_value=0x0000;

static int glb_fiforet_stat_memory_status=0;
static DD_FEC_FIFO_DATA_16 glb_fiforet_stat_memory_value=0x0000;

static int glb_fiforec_stat_memory_status=0;
static DD_FEC_FIFO_DATA_16 glb_fiforec_stat_memory_value=0x0000;
*/

/* used for SET methods on fifos */
/*
static int glb_is_new_frame_for_fifotra=1;
static int glb_new_frame_for_fifotra_length=0;
static int glb_frame_for_fifotra_processed_length=0;


static int glb_is_new_frame_for_fiforet=1;
static int glb_new_frame_for_fiforet_length=0;
static int glb_frame_for_fiforet_processed_length=0;

static int glb_is_new_frame_for_fiforec=1;
static int glb_new_frame_for_fiforec_length=0;
static int glb_frame_for_fiforec_processed_length=0;
*/

/*
Beginning of block-define for STD to 32 conversion functions.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
WARNING   WARNING   WARNING
These blocks are valid ONLY if standard word is 16 bits length
as defined in very first FEC strings protocol.
(should always be the case, but well...)
WARNING   WARNING   WARNING
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

//Local use only ; this function is not exported in glue.h
DD_TYPE_ERROR dd_cat_from_std_to_32(DD_FEC_STD_WORD *fstd_buffer, DD_FEC_FIFO_DATA_32 *f32_buffer, int fstd_length, int *f32_length)
{
int fstd_index, f32_index;
DD_FEC_FIFO_DATA_32 one_32_word, full_32_word;
DD_FEC_STD_WORD one_std_word;


	#ifdef DEBUG_dd_cat_from_std_to_32
		printk("Processing dd_cat_from_std_to_32\n");
	#endif
	fstd_index=0;
	f32_index=0;
	do
	{
		full_32_word = 0x0;
		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 1 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			one_32_word = one_32_word << 24;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}

		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 2 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			one_32_word = one_32_word << 16;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}


		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 3 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			one_32_word = one_32_word << 8;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}

		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 4 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}
		f32_buffer[f32_index] = full_32_word;
		f32_index++;
	} while (fstd_index < fstd_length);
	(*f32_length)=f32_index;
	#ifdef DEBUG_dd_cat_from_std_to_32
		printk("The 32 bits frame is 0x%x words long\n",(*f32_length));
	#endif
return DD_RETURN_OK;
}

/*

static DD_TYPE_ERROR dd_uncat_from_32_to_std(DD_FEC_STD_WORD *fstd_buffer, DD_FEC_FIFO_DATA_32 *f32_buffer, int *fstd_length, int f32_length)
{
//#define DEBUG_dd_uncat_from_32_to_std
int fstd_index, f32_index;  
DD_FEC_FIFO_DATA_32 one_32_word;
DD_FEC_STD_WORD one_std_word;

	#ifdef DEBUG_dd_uncat_from_32_to_std
		printk("Processing dd_uncat_from_32_to_std\n");
	#endif
	fstd_index=0;
	f32_index=0;
	do
	{
		#ifdef DEBUG_dd_uncat_from_32_to_std
			printk("In loop\n");
		#endif

		if (f32_index < f32_length)
		{
			one_32_word =f32_buffer[f32_index];
			one_32_word = (one_32_word >> 24) & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (24) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;


			one_32_word =f32_buffer[f32_index];
			one_32_word = (one_32_word >> 16) & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (16) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;


			one_32_word =f32_buffer[f32_index];
			one_32_word = (one_32_word >> 8) & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (8) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;


			one_32_word =f32_buffer[f32_index];
			one_32_word = one_32_word & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (0) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;
		}
		f32_index++;
	} while (f32_index < f32_length);

	if (isLongFrame(fstd_buffer[DD_MSG_LEN_OFFSET]))
	{
		(*fstd_length) = ((((fstd_buffer[DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + fstd_buffer[DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
	}
	else
	{
		(*fstd_length) = (fstd_buffer[DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
	}

	#ifdef DEBUG_dd_uncat_from_32_to_std
		printk("Final standard frame length is : 0x%x\n", (*fstd_length));
	#endif

	// Add statusk bits to the last data word
	fstd_buffer[(*fstd_length)-1] = fstd_buffer[(*fstd_length)-1] | (fstd_buffer[(*fstd_length)]<<8);
	#ifdef DEBUG_dd_uncat_from_32_to_std
		printk("Status byte of reconstructed frame will be : 0x%x\n", (fstd_buffer[(*fstd_length)]<<8));
	#endif

return DD_RETURN_OK;
}
*/


DD_TYPE_ERROR glue_internal_read16_onfifotra_send32_toprocess(int param_fd, DD_FEC_FIFO_DATA_32 *param_word)
{
static int glb_fifotra_stat_memory_status=0;
static DD_FEC_FIFO_DATA_16 glb_fifotra_stat_memory_value=0x0000;
DD_TYPE_ERROR lcl_err;
DD_FEC_FIFO_DATA_16 param_fifotra16;
DD_FEC_ARRAY lcl_fec_offset_and_value16 = {DD_FEC_FIFOTRA_OFFSET, (DD_FEC_UNIFIED_DATA)&param_fifotra16};
DD_FEC_FIFO_DATA_16 word1, word2, word3, word4;
DD_FEC_FIFO_DATA_32 full_32_word, one_32_word;

	/* must read 4 words of 16 bits and cast them to one word of 32 bits */
	/* first of all, check if we do not have an end status to send as new 32 bits word */
	if (glb_fifotra_stat_memory_status == 1)
	{
		#ifdef DEBUG_R16_S32
			printf("No readout requested from fifotransmit, I will only send the memorized status\n");
		#endif
		word1 = glb_fifotra_stat_memory_value;
		word2 = 0xFFFF;
		word3 = 0xFFFF;
		word4 = 0xFFFF;
		glb_fifotra_stat_memory_status = 0;
	}
	else
	{
		/* Read FIFO word 1 */
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		word1 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
		#ifdef DEBUG_R16_S32
			printf("Reading word1 from fifotransmit :0x%x\n",word1);
		#endif
		/* if we got the last word, including the status */
		if ((word1 & 0xFF00) != 0)
		{
			word2 = (word1 & 0xFF00) >> 8;
			word3 = 0xFFFF;
			word4 = 0xFFFF;
		}
		else
		{
			/* Read FIFO word 2 */
			lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
			if (lcl_err != DD_RETURN_OK) return -1;
			word2 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
			#ifdef DEBUG_R16_S32
				printf("Reading word2 from fifotransmit :0x%x\n",word2);
			#endif
			if ((word2 & 0xFF00) != 0)
			{
				word3 = (word2 & 0xFF00) >> 8;
				word4 = 0xFFFF;
			}
			else
			{
				/* Read FIFO word 3 */
				lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;
				word3 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
				#ifdef DEBUG_R16_S32
					printf("Reading word3 from fifotransmit :0x%x\n",word3);
				#endif
				if ((word3 & 0xFF00) != 0)
				{
					word4 = (word3 & 0xFF00) >> 8;
				}
				else
				{
					/* Read FIFO word 4 */
					lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
					if (lcl_err != DD_RETURN_OK) return -1;
					word4 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
					#ifdef DEBUG_R16_S32
						printf("Reading word4 from fifotransmit :0x%x\n",word4);
					#endif
					if ((word4 & 0xFF00) != 0)
					{
						glb_fifotra_stat_memory_value = (word4 & 0xFF00) >> 8;
						glb_fifotra_stat_memory_status = 1;
					}
				}
			}
		}
	}
		

	/* Now, cat 4 words into 1 single 32 bits word */
	/* initialize full word which will be returned */
	full_32_word = 0x0;
		
	/* process word 1 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 1 of fifotransmit. Word is 0x%x\n",word1);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word1 & 0xFF;
		one_32_word = one_32_word << 24;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 2 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 2 of fifotransmit. Word is 0x%x\n",word2);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word2 & 0xFF;
		one_32_word = one_32_word << 16;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 3 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 3 of fifotransmit. Word is 0x%x\n",word3);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word3 & 0xFF;
		one_32_word = one_32_word << 8;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 4 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 4 of fifotransmit. Word is 0x%x\n",word4);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word4 & 0xFF;
		one_32_word = one_32_word;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	*param_word = full_32_word;
	#ifdef DEBUG_R16_S32
		printf("Sending back to caller, from fifotransmit : Full word : 0x%x\n",full_32_word);
	#endif	
	return DD_RETURN_OK;
}



DD_TYPE_ERROR glue_internal_read16_onfiforet_send32_toprocess(int param_fd, DD_FEC_FIFO_DATA_32 *param_word)
{
static int glb_fiforet_stat_memory_status=0;
static DD_FEC_FIFO_DATA_16 glb_fiforet_stat_memory_value=0x0000;
DD_TYPE_ERROR lcl_err;
DD_FEC_FIFO_DATA_16 param_fiforet16;
DD_FEC_ARRAY lcl_fec_offset_and_value16 = {DD_FEC_FIFORET_OFFSET, (DD_FEC_UNIFIED_DATA)&param_fiforet16};
DD_FEC_FIFO_DATA_16 word1, word2, word3, word4;
DD_FEC_FIFO_DATA_32 full_32_word, one_32_word;

	/* must read 4 words of 16 bits and cast them to one word of 32 bits */
	/* first of all, check if we do not have an end status to send as new 32 bits word */
	if (glb_fiforet_stat_memory_status == 1)
	{
		#ifdef DEBUG_R16_S32
			printf("No readout requested from fiforeturn, I will only send the memorized status\n");
		#endif
		word1 = glb_fiforet_stat_memory_value;
		word2 = 0xFFFF;
		word3 = 0xFFFF;
		word4 = 0xFFFF;
		glb_fiforet_stat_memory_status = 0;
	}
	else
	{
		/* Read FIFO word 1 */
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		word1 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
		#ifdef DEBUG_R16_S32
			printf("Reading word1 from fiforeturn :0x%x\n",word1);
		#endif
		/* if we got the last word, including the status */
		if ((word1 & 0xFF00) != 0)
		{
			word2 = (word1 & 0xFF00) >> 8;
			word3 = 0xFFFF;
			word4 = 0xFFFF;
		}
		else
		{
			/* Read FIFO word 2 */
			lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
			if (lcl_err != DD_RETURN_OK) return -1;
			word2 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
			#ifdef DEBUG_R16_S32
				printf("Reading word2 from fiforeturn :0x%x\n",word2);
			#endif
			if ((word2 & 0xFF00) != 0)
			{
				word3 = (word2 & 0xFF00) >> 8;
				word4 = 0xFFFF;
			}
			else
			{
				/* Read FIFO word 3 */
				lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;
				word3 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
				#ifdef DEBUG_R16_S32
					printf("Reading word3 from fiforeturn :0x%x\n",word3);
				#endif
				if ((word3 & 0xFF00) != 0)
				{
					word4 = (word3 & 0xFF00) >> 8;
				}
				else
				{
					/* Read FIFO word 4 */
					lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
					if (lcl_err != DD_RETURN_OK) return -1;
					word4 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
					#ifdef DEBUG_R16_S32
						printf("Reading word4 from fiforeturn :0x%x\n",word4);
					#endif
					if ((word4 & 0xFF00) != 0)
					{
						glb_fiforet_stat_memory_value = (word4 & 0xFF00) >> 8;
						glb_fiforet_stat_memory_status = 1;
					}
				}
			}
		}
	}
		

	/* Now, cat 4 words into 1 single 32 bits word */
	/* initialize full word which will be returned */
	full_32_word = 0x0;
		
	/* process word 1 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 1 of fiforeturn. Word is 0x%x\n",word1);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word1 & 0xFF;
		one_32_word = one_32_word << 24;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 2 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 2 of fiforeturn. Word is 0x%x\n",word2);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word2 & 0xFF;
		one_32_word = one_32_word << 16;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 3 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 3 of fiforeturn. Word is 0x%x\n",word3);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word3 & 0xFF;
		one_32_word = one_32_word << 8;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 4 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 4 of fiforeturn. Word is 0x%x\n",word4);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word4 & 0xFF;
		one_32_word = one_32_word;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	*param_word = full_32_word;
	#ifdef DEBUG_R16_S32
		printf("Sending back to caller, from fiforeturn : Full word : 0x%x\n",full_32_word);
	#endif	
	return DD_RETURN_OK;
}






DD_TYPE_ERROR glue_internal_read16_onfiforec_send32_toprocess(int param_fd, DD_FEC_FIFO_DATA_32 *param_word)
{
static int glb_fiforec_stat_memory_status=0;
static DD_FEC_FIFO_DATA_16 glb_fiforec_stat_memory_value=0x0000;
DD_TYPE_ERROR lcl_err;
DD_FEC_FIFO_DATA_16 param_fiforec16;
DD_FEC_ARRAY lcl_fec_offset_and_value16 = {DD_FEC_FIFOREC_OFFSET, (DD_FEC_UNIFIED_DATA)&param_fiforec16};
DD_FEC_FIFO_DATA_16 word1, word2, word3, word4;
DD_FEC_FIFO_DATA_32 full_32_word, one_32_word;

	/* must read 4 words of 16 bits and cast them to one word of 32 bits */
	/* first of all, check if we do not have an end status to send as new 32 bits word */
	if (glb_fiforec_stat_memory_status == 1)
	{
		#ifdef DEBUG_R16_S32
			printf("No readout requested from fiforeceive, I will only send the memorized status\n");
		#endif
		word1 = glb_fiforec_stat_memory_value;
		word2 = 0xFFFF;
		word3 = 0xFFFF;
		word4 = 0xFFFF;
		glb_fiforec_stat_memory_status = 0;
	}
	else
	{
		/* Read FIFO word 1 */
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		word1 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
		#ifdef DEBUG_R16_S32
			printf("Reading word1 from fiforeceive :0x%x\n",word1);
		#endif
		/* if we got the last word, including the status */
		if ((word1 & 0xFF00) != 0)
		{
			word2 = (word1 & 0xFF00) >> 8;
			word3 = 0xFFFF;
			word4 = 0xFFFF;
		}
		else
		{
			/* Read FIFO word 2 */
			lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
			if (lcl_err != DD_RETURN_OK) return -1;
			word2 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
			#ifdef DEBUG_R16_S32
				printf("Reading word2 from fiforeceive :0x%x\n",word2);
			#endif
			if ((word2 & 0xFF00) != 0)
			{
				word3 = (word2 & 0xFF00) >> 8;
				word4 = 0xFFFF;
			}
			else
			{
				/* Read FIFO word 3 */
				lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;
				word3 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
				#ifdef DEBUG_R16_S32
					printf("Reading word3 from fiforeceive :0x%x\n",word3);
				#endif
				if ((word3 & 0xFF00) != 0)
				{
					word4 = (word3 & 0xFF00) >> 8;
				}
				else
				{
					/* Read FIFO word 4 */
					lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value16);
					if (lcl_err != DD_RETURN_OK) return -1;
					word4 = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX];
					#ifdef DEBUG_R16_S32
						printf("Reading word4 from fiforeceive :0x%x\n",word4);
					#endif
					if ((word4 & 0xFF00) != 0)
					{
						glb_fiforec_stat_memory_value = (word4 & 0xFF00) >> 8;
						glb_fiforec_stat_memory_status = 1;
					}
				}
			}
		}
	}
		

	/* Now, cat 4 words into 1 single 32 bits word */
	/* initialize full word which will be returned */
	full_32_word = 0x0;
		
	/* process word 1 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 1 of fiforeceive. Word is 0x%x\n",word1);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word1 & 0xFF;
		one_32_word = one_32_word << 24;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 2 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 2 of fiforeceive. Word is 0x%x\n",word2);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word2  & 0xFF;
		one_32_word = one_32_word << 16;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 3 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 3 of fiforeceive. Word is 0x%x\n",word3);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word3 & 0xFF;
		one_32_word = one_32_word << 8;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	/* process word 4 */
	if (word1 != 0xFFFF)
	{
		one_32_word = 0x0;
		#ifdef DEBUG_R16_S32
			printf("Processing word 4 of fiforeceive. Word is 0x%x\n",word4);
		#endif
		one_32_word = (DD_FEC_FIFO_DATA_32)word4 & 0xFF;
		one_32_word = one_32_word;
		full_32_word = full_32_word | one_32_word;
		#ifdef DEBUG_R16_S32
			printf("Full word is now 0x%x\n",full_32_word);
		#endif
	}

	*param_word = full_32_word;
	#ifdef DEBUG_R16_S32
		printf("Sending back to caller, from fiforeceive : Full word : 0x%x\n",full_32_word);
	#endif	

	return DD_RETURN_OK;
}






DD_TYPE_ERROR glue_internal_read32_fromprocess_write16_tofifotra(int param_fd, DD_FEC_FIFO_DATA_32 param_word)
{
static int glb_is_new_frame_for_fifotra=1;
static int glb_new_frame_for_fifotra_length=0;
static int glb_frame_for_fifotra_processed_length=0;
DD_FEC_ARRAY lcl_fec_offset_and_value16 = {DD_FEC_FIFOTRA_OFFSET, 0};
DD_FEC_FIFO_DATA_32 lcl_word32;
DD_FEC_FIFO_DATA_32 word1, word2, word3, word4, word16;
int lcl_count;
DD_TYPE_ERROR lcl_err;
int lcl_i;


	#ifdef DEBUG_G32_W16
		printf("I will write to fiotransmit the word : 0x%x\n", param_word);
	#endif
	/* First of all, decode 32 bits word */
	/* process word1 */
	lcl_word32 = (param_word >> 24) & 0xFF;
	word1 = (DD_FEC_STD_WORD)lcl_word32;
	#ifdef DEBUG_G32_W16
		printf("word1 for fifotransmit is : 0x%x\n", word1);
	#endif
	/* process word2 */
	lcl_word32 = (param_word >> 16) & 0xFF;
	word2 = (DD_FEC_STD_WORD)lcl_word32;
	#ifdef DEBUG_G32_W16
		printf("word2 for fifotransmit is : 0x%x\n", word2);
	#endif
	/* process word3 */
	lcl_word32 = (param_word >> 8) & 0xFF;
	word3 = (DD_FEC_STD_WORD)lcl_word32;
	#ifdef DEBUG_G32_W16
		printf("word3 for fifotransmit is : 0x%x\n", word3);
	#endif
	/* process word4 */
	lcl_word32 = param_word & 0xFF;
	word4 = (DD_FEC_STD_WORD)lcl_word32;
	#ifdef DEBUG_G32_W16
		printf("word4 for fifotransmit is : 0x%x\n", word4);
	#endif

	/* if we are processing a new frame, compute theoretical 16 bits frame length */
	if (glb_is_new_frame_for_fifotra == 1)
	{
		#ifdef DEBUG_G32_W16
			printf("Begin a new frame for fifotransmit\n");
		#endif

		/* compute frame length */
		if (isLongFrame(word3))
		{
			lcl_count = ((((word3 & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + word4) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else lcl_count = (word3 + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);

		/* keep memory of frame length */
		glb_new_frame_for_fifotra_length = lcl_count;
		
		#ifdef DEBUG_G32_W16
			printf("Frame length is : %d\n",glb_new_frame_for_fifotra_length);
		#endif


		/* keep memory of how many words have alreday been processed for this frame */
		glb_frame_for_fifotra_processed_length = 4;
		
		/* send the words to fifo transmit */
		/* send word 1 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word1;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 2 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word2;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 3 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word3;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 4 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word4;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		
		/* reset is_new_frame flag */
		glb_is_new_frame_for_fifotra = 0;
		
		#ifdef DEBUG_G32_W16
			printf("%d words have been written in fifotransmit\n",glb_frame_for_fifotra_processed_length);
		#endif
		
		return DD_RETURN_OK;

	}
	else
	{
		/* if we have already processed one 32 bits word, i.e. the first 4 words of 16 bits
		constituting the beginning of a frame */
	
		for (lcl_i=0; lcl_i<4;lcl_i++)
		{
		
			if (lcl_i==0) word16=word1;
			if (lcl_i==1) word16=word2;
			if (lcl_i==2) word16=word3;
			if (lcl_i==3) word16=word4;
			
			#ifdef DEBUG_G32_W16
				printf("In fifotransmit loop, Now processing word %d = 0x%x\n",lcl_i, word16);
			#endif

					
			/* if next word (word16) must be the last one of the frame */
			if ( glb_new_frame_for_fifotra_length == (glb_frame_for_fifotra_processed_length+1) )
			{
				/* set EOF tag on word 1 */
				word16 = (word16 | 0x8000);
				#ifdef DEBUG_G32_W16
					printf("The word %d will terminate the frame as : 0x%x\n",lcl_i, word16);
				#endif

				/* write word1 in fifo */
				lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word16;
				lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;
			
				/* reset fifotra status */
				glb_is_new_frame_for_fifotra = 1;
				glb_frame_for_fifotra_processed_length=0;
				glb_new_frame_for_fifotra_length=0;

				#ifdef DEBUG_G32_W16
					printf("Done, word %d = 0x%x wrote in fifotransmit\n",lcl_i, word16);
				#endif

				/* exit function */
				return DD_RETURN_OK;
			}
			else
			{
				/* write word16 in fifo */
				lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word16;
				lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;

				/* inc processed words counter */
				glb_frame_for_fifotra_processed_length++;
				#ifdef DEBUG_G32_W16
					printf("Done, word %d = 0x%x wrote in fifotransmit\n",lcl_i, word16);
					printf("I have wrote %d elements in fifotransmit for this frame\n",glb_frame_for_fifotra_processed_length);
				#endif

			}
		}
		return DD_RETURN_OK;
	}
return -1;
}


DD_TYPE_ERROR glue_internal_read32_fromprocess_write16_tofiforet(int param_fd, DD_FEC_FIFO_DATA_32 param_word)
{
static int glb_is_new_frame_for_fiforet=1;
static int glb_new_frame_for_fiforet_length=0;
static int glb_frame_for_fiforet_processed_length=0;
DD_FEC_ARRAY lcl_fec_offset_and_value16 = {DD_FEC_FIFORET_OFFSET, 0};
DD_FEC_FIFO_DATA_32 lcl_word32;
DD_FEC_FIFO_DATA_32 word1, word2, word3, word4, word16;
int lcl_count;
DD_TYPE_ERROR lcl_err;
int lcl_i;


	/* First of all, decode 32 bits word */
	/* process word1 */
	lcl_word32 = (param_word >> 24) & 0xFF;
	word1 = (DD_FEC_STD_WORD)lcl_word32;
	/* process word2 */
	lcl_word32 = (param_word >> 16) & 0xFF;
	word2 = (DD_FEC_STD_WORD)lcl_word32;
	/* process word3 */
	lcl_word32 = (param_word >> 8) & 0xFF;
	word3 = (DD_FEC_STD_WORD)lcl_word32;
	/* process word4 */
	lcl_word32 = param_word & 0xFF;
	word4 = (DD_FEC_STD_WORD)lcl_word32;
	
	/* if we are processing a new frame, compute theoretical 16 bits frame length */
	if (glb_is_new_frame_for_fiforet == 1)
	{

		/* compute frame length */
		if (isLongFrame(word3))
		{
			lcl_count = ((((word3 & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + word4) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else lcl_count = (word3 + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);

		/* keep memory of frame length */
		glb_new_frame_for_fiforet_length = lcl_count;

		/* keep memory of how many words have alreday been processed for this frame */
		glb_frame_for_fiforet_processed_length = 4;
		
		/* send the words to fifo transmit */
		/* send word 1 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word1;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 2 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word2;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 3 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word3;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 4 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word4;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		
		/* reset is_new_frame flag */
		glb_is_new_frame_for_fiforet = 0;
		return DD_RETURN_OK;

	}
	else
	{
		/* if we have already processed one 32 bits word, i.e. the first 4 words of 16 bits
		constituting the beginning of a frame */
	
		for (lcl_i=0; lcl_i<4;lcl_i++)
		{
		
			if (lcl_i==0) word16=word1;
			if (lcl_i==1) word16=word2;
			if (lcl_i==2) word16=word3;
			if (lcl_i==3) word16=word4;
					
			/* if next word (word16) must be the last one of the frame */
			if ( glb_new_frame_for_fiforet_length == (glb_frame_for_fiforet_processed_length+1) )
			{
				/* set EOF tag on word 1 */
				word16 = (word16 | 0x8000);
			
				/* write word1 in fifo */
				lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word16;
				lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;
			
				/* reset fifotra status */
				glb_is_new_frame_for_fiforet = 1;
				glb_frame_for_fiforet_processed_length=0;
				glb_new_frame_for_fiforet_length=0;
			
				/* exit function */
				return DD_RETURN_OK;
			}
			else
			{
				/* write word16 in fifo */
				lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word16;
				lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;

				/* inc processed words counter */
				glb_frame_for_fiforet_processed_length++;
			}
		}
		return DD_RETURN_OK;
	}
return -1;
}



DD_TYPE_ERROR glue_internal_read32_fromprocess_write16_tofiforec(int param_fd, DD_FEC_FIFO_DATA_32 param_word)
{
static int glb_is_new_frame_for_fiforec=1;
static int glb_new_frame_for_fiforec_length=0;
static int glb_frame_for_fiforec_processed_length=0;
DD_FEC_ARRAY lcl_fec_offset_and_value16 = {DD_FEC_FIFOREC_OFFSET, 0};
DD_FEC_FIFO_DATA_32 lcl_word32;
DD_FEC_FIFO_DATA_32 word1, word2, word3, word4, word16;
int lcl_count;
DD_TYPE_ERROR lcl_err;
int lcl_i;


	/* First of all, decode 32 bits word */
	/* process word1 */
	lcl_word32 = (param_word >> 24) & 0xFF;
	word1 = (DD_FEC_STD_WORD)lcl_word32;
	/* process word2 */
	lcl_word32 = (param_word >> 16) & 0xFF;
	word2 = (DD_FEC_STD_WORD)lcl_word32;
	/* process word3 */
	lcl_word32 = (param_word >> 8) & 0xFF;
	word3 = (DD_FEC_STD_WORD)lcl_word32;
	/* process word4 */
	lcl_word32 = param_word & 0xFF;
	word4 = (DD_FEC_STD_WORD)lcl_word32;
	
	/* if we are processing a new frame, compute theoretical 16 bits frame length */
	if (glb_is_new_frame_for_fiforec == 1)
	{

		/* compute frame length */
		if (isLongFrame(word3))
		{
			lcl_count = ((((word3 & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + word4) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else lcl_count = (word3 + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);

		/* keep memory of frame length */
		glb_new_frame_for_fiforec_length = lcl_count;

		/* keep memory of how many words have alreday been processed for this frame */
		glb_frame_for_fiforec_processed_length = 4;
		
		/* send the words to fifo transmit */
		/* send word 1 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word1;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 2 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word2;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 3 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word3;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		/* send word 4 */
		lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word4;
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
		if (lcl_err != DD_RETURN_OK) return -1;
		
		/* reset is_new_frame flag */
		glb_is_new_frame_for_fiforec = 0;
		return DD_RETURN_OK;

	}
	else
	{
		/* if we have already processed one 32 bits word, i.e. the first 4 words of 16 bits
		constituting the beginning of a frame */
	
		for (lcl_i=0; lcl_i<4;lcl_i++)
		{
		
			if (lcl_i==0) word16=word1;
			if (lcl_i==1) word16=word2;
			if (lcl_i==2) word16=word3;
			if (lcl_i==3) word16=word4;
					
			/* if next word (word16) must be the last one of the frame */
			if ( glb_new_frame_for_fiforec_length == (glb_frame_for_fiforec_processed_length+1) )
			{
				/* set EOF tag on word 1 */
				word16 = (word16 | 0x8000);
			
				/* write word1 in fifo */
				lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word16;
				lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;
			
				/* reset fifotra status */
				glb_is_new_frame_for_fiforec = 1;
				glb_frame_for_fiforec_processed_length=0;
				glb_new_frame_for_fiforec_length=0;
			
				/* exit function */
				return DD_RETURN_OK;
			}
			else
			{
				/* write word16 in fifo */
				lcl_fec_offset_and_value16[DD_FEC_VALUE_INDEX]= word16;
				lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value16);
				if (lcl_err != DD_RETURN_OK) return -1;

				/* inc processed words counter */
				glb_frame_for_fiforec_processed_length++;
			}
		}
		return DD_RETURN_OK;
	}
return -1;
}





/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_pci_get_command_reg(int param_fd, DD_TYPE_PCI_COMMAND_REG *param_comreg)
{
	return( ioctl(param_fd, DD_IOC_GET_PCI_COMMAND_REG, param_comreg) );
}





/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_raw_ba0(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba0)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_BASE0_ADDRESS, param_ba0) );
}




/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_raw_ba1(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba1)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_BASE1_ADDRESS, param_ba1) );
}


/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_raw_ba2(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba2)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_BASE2_ADDRESS, param_ba2) );
}


/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_raw_ba3(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba3)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_BASE3_ADDRESS, param_ba3) );
}





/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba0(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba0)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_REMAPPED_BASE0_ADDRESS, param_ba0) );
}



/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba1(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba1)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_REMAPPED_BASE1_ADDRESS, param_ba1) );
}


/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba2(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba2)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_REMAPPED_BASE2_ADDRESS, param_ba2) );
}


/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba3(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba3)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_REMAPPED_BASE3_ADDRESS, param_ba3) );
}




DD_TYPE_ERROR glue_plx_hard_reset(int param_fd)
{
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_PLX_HARD_RESET_MODULE);
	usleep(5000);
	return lcl_err;
}


DD_TYPE_ERROR glue_plx_enable_irqs(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_ENABLE_PLX_INTERRUPTS) );
}



DD_TYPE_ERROR glue_plx_disable_irqs(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_DISABLE_PLX_INTERRUPTS) );
}




DD_TYPE_ERROR glue_plx_get_value(int param_fd, DD_PLX_ADDRESS param_offset, DD_PLX_DATA *param_data)
{
DD_PLX_ARRAY lcl_plx_array;
DD_TYPE_ERROR lcl_err = DD_RETURN_OK;

	lcl_plx_array[DD_PLX_OFFSET_INDEX] = param_offset;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_PLX, lcl_plx_array);
	*param_data =lcl_plx_array[DD_PLX_VALUE_INDEX];
return lcl_err;
}









DD_TYPE_ERROR glue_plx_set_value(int param_fd, DD_PLX_ADDRESS param_offset, DD_PLX_DATA param_data)
{
DD_PLX_ARRAY lcl_plx_array;
	lcl_plx_array[DD_PLX_OFFSET_INDEX] = param_offset;
	lcl_plx_array[DD_PLX_VALUE_INDEX] = param_data;
	return( ioctl(param_fd, DD_IOC_WRITE_TO_PLX, lcl_plx_array) );
}




DD_TYPE_ERROR glue_plx_remap_base_addresses(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_PLX_SET_ADDRESSES) );
}








/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_set_ctrl0(int param_fd, DD_FEC_REGISTER_DATA param_control0)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_CTRL0_OFFSET, param_control0};
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, &lcl_fec_offset_and_value) );
}



/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_ctrl0(int param_fd, DD_FEC_REGISTER_DATA *param_control0)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_CTRL0_OFFSET, *param_control0};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_control0 = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}



/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_set_ctrl1(int param_fd, DD_FEC_REGISTER_DATA param_control1)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_CTRL1_OFFSET, param_control1};
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, &lcl_fec_offset_and_value) );
}


/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_ctrl1(int param_fd, DD_FEC_REGISTER_DATA *param_control1)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_CTRL1_OFFSET, *param_control1};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_control1 = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}



/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_status0(int param_fd, DD_FEC_REGISTER_DATA *param_status0)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_STAT0_OFFSET, *param_status0};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_status0 = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}




/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_status1(int param_fd, DD_FEC_REGISTER_DATA *param_status1)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_STAT1_OFFSET, *param_status1};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_status1 = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}



/* Field added 17/01/2005 - This function exists only on 32 bits FECs */
/*!
Glue functions ; names are explicit.
*/
/*
DD_TYPE_ERROR glue_fec_get_source_register(int param_fd, DD_FEC_REGISTER_DATA *param_source)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIRMWARE_DATE_OFFSET, *param_source};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_source = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}
*/

/* Field added 17/01/2005 - This function exists only on 32 bits FECs */
/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_version_register(int param_fd, DD_FEC_REGISTER_DATA *param_version)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIRMWARE_VERSION_OFFSET, *param_version};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_version = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}


//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
/* FIFO TRANSMIT BLOCK */
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::

	/*!
	Glue functions ; names are explicit.
	*/
	/* if we have 32 bits fifo's */
	DD_TYPE_ERROR glue_fec_set_frame_in_fifotra_32(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength)
	{
	DD_FEC_FIFO_DATA_32 lcl_frame32[DD_MAX_MSG_LENGTH_32];
	int lcl_length32;
	int lcl_i;
	DD_FEC_ARRAY lcl_fec_offset_and_value;
	DD_TYPE_ERROR lcl_err;


		/* check if we can convert the frame or is it too long ? */
		if (param_stdframelength > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;

		/* convert the frame to 32 bits words */
		dd_cat_from_std_to_32(param_stdframecontent, lcl_frame32, param_stdframelength, &lcl_length32);

		/* Write the frame in fifo */
		for (lcl_i=0; lcl_i<lcl_length32 ; lcl_i++)
		{
			lcl_fec_offset_and_value[DD_FEC_OFFSET_INDEX]= (DD_FEC_UNIFIED_DATA)DD_FEC_FIFOTRA_OFFSET;
			lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX]= (DD_FEC_UNIFIED_DATA)lcl_frame32[lcl_i];
			lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
			if (lcl_err != DD_RETURN_OK) return lcl_err;
		}

		return DD_RETURN_OK;
	}


	/*!
	Glue functions ; names are explicit.
	*/
	/* if we have 16 bits fifo's */
	DD_TYPE_ERROR glue_fec_set_frame_in_fifotra_16(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength)
	{
	DD_FEC_ARRAY lcl_fec_offset_and_value;
	int lcl_i;
	DD_TYPE_ERROR lcl_err;

		for (lcl_i=0; lcl_i<param_stdframelength ; lcl_i++)
		{
			lcl_fec_offset_and_value[DD_FEC_OFFSET_INDEX]= (DD_FEC_UNIFIED_DATA)DD_FEC_FIFOTRA_OFFSET;
			lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX]= (DD_FEC_UNIFIED_DATA)param_stdframecontent[lcl_i];
			lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
			if (lcl_err != DD_RETURN_OK) return lcl_err;
		}
		return DD_RETURN_OK;
	}









/*!
Glue functions ; names are explicit.
*/
/*
DD_TYPE_ERROR glue_fec_set_native_fifotra_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fifotra)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, param_fifotra};
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value) );
}
*/

DD_TYPE_ERROR glue_fec_set_native_fifotra_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fifotra)
{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, param_fifotra};
	DD_TYPE_ERROR lcl_err;
	int lcl_size;

	/* this function is called by upper layers, whatever the kind of FEC we have.
	Therefore, if the FEC we drive is REALLY 32 bits then : np, direct write
	else, if if the FEC we drive is NOT 32 bits but IS 16 bits, we will have to fake it */

	/* First, check with which kind of fifo depth we have to deal with */
	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;

	/* if we have native 32 bits fifo */
	if (lcl_size == 32)
	{
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
		return lcl_err;
	}
	else
	/* if we have native 16 bits fifo */
	{
		lcl_err = glue_internal_read32_fromprocess_write16_tofifotra(param_fd, param_fifotra);
		return lcl_err;
	}	
	

return -1;

}




















	DD_TYPE_ERROR glue_fec_set_native_fifotra_item_16(int param_fd, DD_FEC_FIFO_DATA_16 param_fifotra)
	{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, param_fifotra};
		return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value) );
	}






/*!
Glue functions ; names are explicit.
Allow to read a 32 bits word with and old 16 bits capable call.
*/
DD_TYPE_ERROR glue_fec_get_fifotra_item_32(int param_fd, DD_FEC_STD_WORD *param_fifotra)
{
//#define DEBUG_glue_fec_get_fifotra

static int nb_32bits_words_read=0;
static int total_nb_std_words_transmitted=0;
static int partial_nb_std_words_transmitted=0;
static DD_FEC_FIFO_DATA_32 current_32bits_word=0x0;
static int lcl_frame_length;
static DD_FEC_STD_WORD word0, word1, word2, word3;

DD_FEC_STD_WORD lcl_fl1, lcl_fl2;

DD_FEC_STD_WORD status0=0x0;
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, *param_fifotra};
DD_TYPE_ERROR lcl_err=DD_RETURN_OK;
DD_FEC_ARRAY lcl_fec_offset_and_value_for_status;



	#ifdef DEBUG_glue_fec_get_fifotra
		printf("Entering function.\n");
		printf("nb_32bits_words_read=%d, total_nb_std_words_transmitted=%d, partial_nb_std_words_transmitted=%d\n",nb_32bits_words_read,total_nb_std_words_transmitted,partial_nb_std_words_transmitted);
	#endif


	//If we have to process a new 32 bits word
	if (partial_nb_std_words_transmitted==0)
	{

		//first, check if we have something in our fifo receive
		lcl_fec_offset_and_value_for_status[0]=DD_FEC_STAT0_OFFSET;
		lcl_fec_offset_and_value_for_status[1]=status0;
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value_for_status);
		if (lcl_err != DD_RETURN_OK)
		{
			partial_nb_std_words_transmitted = 0 ;
			total_nb_std_words_transmitted = 0;
			nb_32bits_words_read = 0;
			return lcl_err;
		}
		status0 = lcl_fec_offset_and_value_for_status[DD_FEC_VALUE_INDEX];
		#ifdef DEBUG_glue_fec_get_fifotra
			printf("fec status register 0 is : 0x%x\n", status0);
		#endif

		if (status0 & DD_FEC_REC_EMPTY)
		{
			#ifdef DEBUG_glue_fec_get_fifotra
				printf("Fifo receive detected as empty\n");
			#endif
			*param_fifotra =0xFFFF;
			return DD_RETURN_OK;
		}





		#ifdef DEBUG_glue_fec_get_fifotra
			printf("Read 32 bits word requested\n");
		#endif
		//read the 32 bits word
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		if (lcl_err != DD_RETURN_OK)
		{
			partial_nb_std_words_transmitted = 0 ;
			total_nb_std_words_transmitted = 0;
			nb_32bits_words_read = 0;
			return lcl_err;
		}
		current_32bits_word = lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		nb_32bits_words_read++;
		#ifdef DEBUG_glue_fec_get_fifotra
			printf("I have read : 0x%x\n",current_32bits_word);
		#endif

		//If we are reading the first 32 bits word,
		//perform a frame length analysis
		if (nb_32bits_words_read == 1)
		{
			lcl_fl1 = (current_32bits_word >> 8) & 0xFF;
			lcl_fl2 = current_32bits_word & 0xFF;
			#ifdef DEBUG_glue_fec_get_fifotra
				printf("My first length word is : 0x%x\n",lcl_fl1);
				printf("My second length word is : 0x%x\n",lcl_fl2);
			#endif


			if ((lcl_fl1 & 0x0080)==0)
			{
				#ifdef DEBUG_glue_fec_get_fifotra
					printf("I'm in short frame case\n");
				#endif
				lcl_frame_length = lcl_fl1 +3;
			}
			else
			{
				lcl_frame_length = ((((lcl_fl2 & 0x007F) << 8) + lcl_fl1) +4);
				#ifdef DEBUG_glue_fec_get_fifotra
					printf("I'm in long frame case\n");
				#endif
			}
			lcl_frame_length++;
		}

		#ifdef DEBUG_glue_fec_get_fifotra
			printf("I see a lcl_frame_length of %d\n",lcl_frame_length);
		#endif

		//in all cases, if we are processing a new 32 bits word,
		//prepare the 16 bits words to send back to the calling function
		word0 = (current_32bits_word >> 24) & 0xFF;
		word1 = (current_32bits_word >> 16) & 0xFF;
		word2 = (current_32bits_word >> 8) & 0xFF;
		word3 = current_32bits_word & 0xFF;
	}

	//Now, check what we must transmit and how
	//First, if we have no need to play with the status byte
	if (total_nb_std_words_transmitted < (lcl_frame_length-2))
	{
		switch (partial_nb_std_words_transmitted)
		{
			case 0:
				partial_nb_std_words_transmitted =1 ;
				total_nb_std_words_transmitted++;
				*param_fifotra = word0;
				break;

			case 1:
				partial_nb_std_words_transmitted =2 ;
				total_nb_std_words_transmitted++;
				*param_fifotra = word1;
				break;

			case 2:
				partial_nb_std_words_transmitted =3 ;
				total_nb_std_words_transmitted++;
				*param_fifotra = word2;
				break;

			case 3:
				partial_nb_std_words_transmitted =0 ;
				total_nb_std_words_transmitted++;
				*param_fifotra = word3;
				break;
		}
	}
	//Else, if the status byte if needed
	else
	{
		#ifdef DEBUG_glue_fec_get_fifotra
			printf("Now, the status byte must be processed\n");
		#endif
		switch (partial_nb_std_words_transmitted)
		{
			case 0:
				*param_fifotra = (word0 | (word1 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fifotra
					printf("I am in case 0\n");
				#endif
				break;

			case 1:
				*param_fifotra = (word1 | (word2 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fifotra
					printf("I am in case 1\n");
				#endif
				break;

			case 2:
				*param_fifotra = (word2 | (word3 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fifotra
					printf("I am in case 2\n");
				#endif
				break;

			case 3:
				lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
				if (lcl_err != DD_RETURN_OK)
				{
					partial_nb_std_words_transmitted = 0 ;
					total_nb_std_words_transmitted = 0;
					nb_32bits_words_read = 0;
					return lcl_err;
				}
				current_32bits_word = lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
				word0 = (current_32bits_word >> 24) & 0xFF;
				*param_fifotra = (word3 | (word0 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fifotra
					printf("I am in case 3\n");
				#endif
				break;
		}

	}

	#ifdef DEBUG_glue_fec_get_fifotra
		printf("Exiting function.\n");
		printf("nb_32bits_words_read=%d, total_nb_std_words_transmitted=%d, partial_nb_std_words_transmitted=%d\n",nb_32bits_words_read,total_nb_std_words_transmitted,partial_nb_std_words_transmitted);
	#endif

	return lcl_err;
}



/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_fifotra_item_16(int param_fd, DD_FEC_STD_WORD *param_fifotra)
{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, *param_fifotra};
	DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
	*param_fifotra = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}



DD_TYPE_ERROR glue_fec_get_fifotra_item(int param_fd, DD_FEC_STD_WORD *param_fifotra)
{
int lcl_size;
DD_TYPE_ERROR lcl_err;

	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;
	if (lcl_size == 16)
	{
		lcl_err = glue_fec_get_fifotra_item_16(param_fd, param_fifotra);
	}
	else lcl_err = glue_fec_get_fifotra_item_32(param_fd, param_fifotra);
	return lcl_err;
}


DD_TYPE_ERROR glue_fec_get_native_fifotra_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fifotra)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, *param_fifotra};
DD_TYPE_ERROR lcl_err;
int lcl_size;
DD_FEC_FIFO_DATA_32 lcl_word_32;

	/* this function is called by upper layers, whatever the kind of FEC we have.
	Therefore, if the FEC we drive is REALLY 32 bits then : np, direct read
	else, if if the FEC we drive is NOT 32 bits but IS 16 bits, we will have to fake it */

	/* First, check with which kind of fifo depth we have to deal with */
	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;

	/* if we have native 32 bits fifo */
	if (lcl_size == 32)
	{
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		*param_fifotra = (DD_FEC_FIFO_DATA_32)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		return lcl_err;
	}
	else
	/* if we have native 16 bits fifo */
	{
		lcl_err = glue_internal_read16_onfifotra_send32_toprocess(param_fd, &lcl_word_32);
		*param_fifotra = lcl_word_32;
		return lcl_err;
	}	
	

return -1;

}

DD_TYPE_ERROR glue_fec_get_native_fifotra_item_16(int param_fd, DD_FEC_FIFO_DATA_16 *param_fifotra)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOTRA_OFFSET, *param_fifotra};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
	*param_fifotra = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}







//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
/* FIFO RETURN BLOCK */
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::


	/*!
	Glue functions ; names are explicit.
	*/
	DD_TYPE_ERROR glue_fec_set_frame_in_fiforet_32(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength)
	{
	DD_FEC_FIFO_DATA_32 lcl_frame32[DD_MAX_MSG_LENGTH_32];
	int lcl_length32;
	int lcl_i;
	DD_FEC_ARRAY lcl_fec_offset_and_value;
	DD_TYPE_ERROR lcl_err;

		/* check if we can convert the frame or is it too long ? */
		if (param_stdframelength > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;

		/* convert the frame to 32 bits words */
		dd_cat_from_std_to_32(param_stdframecontent, lcl_frame32, param_stdframelength, &lcl_length32);

		/* Write the frame in fifo */
		for (lcl_i=0; lcl_i<lcl_length32 ; lcl_i++)
		{
			lcl_fec_offset_and_value[DD_FEC_OFFSET_INDEX]= (DD_FEC_UNIFIED_DATA)DD_FEC_FIFORET_OFFSET;
			lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX]= (DD_FEC_UNIFIED_DATA)lcl_frame32[lcl_i];
			lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
			if (lcl_err != DD_RETURN_OK) return lcl_err;
		}

		return DD_RETURN_OK;
	}


	/*!
	Glue functions ; names are explicit.
	*/
	DD_TYPE_ERROR glue_fec_set_frame_in_fiforet_16(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength)
	{
	DD_FEC_ARRAY lcl_fec_offset_and_value;
	int lcl_i;
	DD_TYPE_ERROR lcl_err;

		for (lcl_i=0; lcl_i<param_stdframelength ; lcl_i++)
		{
			lcl_fec_offset_and_value[DD_FEC_OFFSET_INDEX]= (DD_FEC_UNIFIED_DATA)DD_FEC_FIFORET_OFFSET;
			lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX]= (DD_FEC_UNIFIED_DATA)param_stdframecontent[lcl_i];
			lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
			if (lcl_err != DD_RETURN_OK) return lcl_err;
		}
		return DD_RETURN_OK;
	}


	/*!
	Glue functions ; names are explicit.
	*/
/*
	DD_TYPE_ERROR glue_fec_set_native_fiforet_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fiforet)
	{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, param_fiforet};
		return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value) );
	}
*/






DD_TYPE_ERROR glue_fec_set_native_fiforet_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fiforet)
{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, param_fiforet};
	DD_TYPE_ERROR lcl_err;
	int lcl_size;

	/* this function is called by upper layers, whatever the kind of FEC we have.
	Therefore, if the FEC we drive is REALLY 32 bits then : np, direct write
	else, if if the FEC we drive is NOT 32 bits but IS 16 bits, we will have to fake it */

	/* First, check with which kind of fifo depth we have to deal with */
	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;

	/* if we have native 32 bits fifo */
	if (lcl_size == 32)
	{
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
		return lcl_err;
	}
	else
	/* if we have native 16 bits fifo */
	{
		lcl_err = glue_internal_read32_fromprocess_write16_tofiforet(param_fd, param_fiforet);
		return lcl_err;
	}	
	

return -1;

}

















	DD_TYPE_ERROR glue_fec_set_native_fiforet_item_16(int param_fd, DD_FEC_FIFO_DATA_16 param_fiforet)
	{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, param_fiforet};
		return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value) );
	}





/*!
Glue functions ; names are explicit.
Allow to read a 32 bits word with and old 16 bits capable call.
*/
DD_TYPE_ERROR glue_fec_get_fiforet_item_32(int param_fd, DD_FEC_STD_WORD *param_fiforet)
{
//#define DEBUG_glue_fec_get_fiforet

static int nb_32bits_words_read=0;
static int total_nb_std_words_transmitted=0;
static int partial_nb_std_words_transmitted=0;
static DD_FEC_FIFO_DATA_32 current_32bits_word=0x0;
static int lcl_frame_length;
static DD_FEC_STD_WORD word0, word1, word2, word3;

DD_FEC_STD_WORD lcl_fl1, lcl_fl2;

DD_FEC_STD_WORD status0=0x0;
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, *param_fiforet};
DD_TYPE_ERROR lcl_err=DD_RETURN_OK;
DD_FEC_ARRAY lcl_fec_offset_and_value_for_status;



	#ifdef DEBUG_glue_fec_get_fiforet
		printf("Entering function.\n");
		printf("nb_32bits_words_read=%d, total_nb_std_words_transmitted=%d, partial_nb_std_words_transmitted=%d\n",nb_32bits_words_read,total_nb_std_words_transmitted,partial_nb_std_words_transmitted);
	#endif


	//If we have to process a new 32 bits word
	if (partial_nb_std_words_transmitted==0)
	{

		//first, check if we have something in our fifo receive
		lcl_fec_offset_and_value_for_status[0]=DD_FEC_STAT0_OFFSET;
		lcl_fec_offset_and_value_for_status[1]=status0;
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value_for_status);
		if (lcl_err != DD_RETURN_OK)
		{
			partial_nb_std_words_transmitted = 0 ;
			total_nb_std_words_transmitted = 0;
			nb_32bits_words_read = 0;
			return lcl_err;
		}
		status0 = lcl_fec_offset_and_value_for_status[DD_FEC_VALUE_INDEX];
		#ifdef DEBUG_glue_fec_get_fiforet
			printf("fec status register 0 is : 0x%x\n", status0);
		#endif

		if (status0 & DD_FEC_REC_EMPTY)
		{
			#ifdef DEBUG_glue_fec_get_fiforet
				printf("Fifo receive detected as empty\n");
			#endif
			*param_fiforet =0xFFFF;
			return DD_RETURN_OK;
		}





		#ifdef DEBUG_glue_fec_get_fiforet
			printf("Read 32 bits word requested\n");
		#endif
		//read the 32 bits word
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		if (lcl_err != DD_RETURN_OK)
		{
			partial_nb_std_words_transmitted = 0 ;
			total_nb_std_words_transmitted = 0;
			nb_32bits_words_read = 0;
			return lcl_err;
		}
		current_32bits_word = lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		nb_32bits_words_read++;
		#ifdef DEBUG_glue_fec_get_fiforet
			printf("I have read : 0x%x\n",current_32bits_word);
		#endif

		//If we are reading the first 32 bits word,
		//perform a frame length analysis
		if (nb_32bits_words_read == 1)
		{
			lcl_fl1 = (current_32bits_word >> 8) & 0xFF;
			lcl_fl2 = current_32bits_word & 0xFF;
			#ifdef DEBUG_glue_fec_get_fiforet
				printf("My first length word is : 0x%x\n",lcl_fl1);
				printf("My second length word is : 0x%x\n",lcl_fl2);
			#endif


			if ((lcl_fl1 & 0x0080)==0)
			{
				#ifdef DEBUG_glue_fec_get_fiforet
					printf("I'm in short frame case\n");
				#endif
				lcl_frame_length = lcl_fl1 +3;
			}
			else
			{
				lcl_frame_length = ((((lcl_fl2 & 0x007F) << 8) + lcl_fl1) +4);
				#ifdef DEBUG_glue_fec_get_fiforet
					printf("I'm in long frame case\n");
				#endif
			}
			lcl_frame_length++;
		}

		#ifdef DEBUG_glue_fec_get_fiforet
			printf("I see a lcl_frame_length of %d\n",lcl_frame_length);
		#endif

		//in all cases, if we are processing a new 32 bits word,
		//prepare the 16 bits words to send back to the calling function
		word0 = (current_32bits_word >> 24) & 0xFF;
		word1 = (current_32bits_word >> 16) & 0xFF;
		word2 = (current_32bits_word >> 8) & 0xFF;
		word3 = current_32bits_word & 0xFF;
	}

	//Now, check what we must transmit and how
	//First, if we have no need to play with the status byte
	if (total_nb_std_words_transmitted < (lcl_frame_length-2))
	{
		switch (partial_nb_std_words_transmitted)
		{
			case 0:
				partial_nb_std_words_transmitted =1 ;
				total_nb_std_words_transmitted++;
				*param_fiforet = word0;
				break;

			case 1:
				partial_nb_std_words_transmitted =2 ;
				total_nb_std_words_transmitted++;
				*param_fiforet = word1;
				break;

			case 2:
				partial_nb_std_words_transmitted =3 ;
				total_nb_std_words_transmitted++;
				*param_fiforet = word2;
				break;

			case 3:
				partial_nb_std_words_transmitted =0 ;
				total_nb_std_words_transmitted++;
				*param_fiforet = word3;
				break;
		}
	}
	//Else, if the status byte if needed
	else
	{
		#ifdef DEBUG_glue_fec_get_fiforet
			printf("Now, the status byte must be processed\n");
		#endif
		switch (partial_nb_std_words_transmitted)
		{
			case 0:
				*param_fiforet = (word0 | (word1 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforet
					printf("I am in case 0\n");
				#endif
				break;

			case 1:
				*param_fiforet = (word1 | (word2 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforet
					printf("I am in case 1\n");
				#endif
				break;

			case 2:
				*param_fiforet = (word2 | (word3 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforet
					printf("I am in case 2\n");
				#endif
				break;

			case 3:
				lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
				if (lcl_err != DD_RETURN_OK)
				{
					partial_nb_std_words_transmitted = 0 ;
					total_nb_std_words_transmitted = 0;
					nb_32bits_words_read = 0;
					return lcl_err;
				}
				current_32bits_word = lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
				word0 = (current_32bits_word >> 24) & 0xFF;
				*param_fiforet = (word3 | (word0 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforet
					printf("I am in case 3\n");
				#endif
				break;
		}

	}

	#ifdef DEBUG_glue_fec_get_fiforet
		printf("Exiting function.\n");
		printf("nb_32bits_words_read=%d, total_nb_std_words_transmitted=%d, partial_nb_std_words_transmitted=%d\n",nb_32bits_words_read,total_nb_std_words_transmitted,partial_nb_std_words_transmitted);
	#endif

	return lcl_err;
}


	/*!
	Glue functions ; names are explicit.
	*/
	DD_TYPE_ERROR glue_fec_get_fiforet_item_16(int param_fd, DD_FEC_STD_WORD *param_fiforet)
	{
		DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, *param_fiforet};
		DD_TYPE_ERROR lcl_err;
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		*param_fiforet = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		return lcl_err;
	}


DD_TYPE_ERROR glue_fec_get_fiforet_item(int param_fd, DD_FEC_STD_WORD *param_fiforet)
{
int lcl_size;
DD_TYPE_ERROR lcl_err;

	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;
	if (lcl_size == 16)
	{
		lcl_err = glue_fec_get_fiforet_item_16(param_fd, param_fiforet);
	}
	else lcl_err = glue_fec_get_fiforet_item_32(param_fd, param_fiforet);
	return lcl_err;
}


/*!
See the public file glue.h for this API description.
*/
/*
DD_TYPE_ERROR glue_fec_get_native_fiforet_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fiforet)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, *param_fiforet};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
	*param_fiforet = (DD_FEC_FIFO_DATA_32)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}
*/



DD_TYPE_ERROR glue_fec_get_native_fiforet_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fiforet)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, *param_fiforet};
DD_TYPE_ERROR lcl_err;
int lcl_size;
DD_FEC_FIFO_DATA_32 lcl_word_32;

	/* this function is called by upper layers, whatever the kind of FEC we have.
	Therefore, if the FEC we drive is REALLY 32 bits then : np, direct read
	else, if if the FEC we drive is NOT 32 bits but IS 16 bits, we will have to fake it */

	/* First, check with which kind of fifo depth we have to deal with */
	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;

	/* if we have native 32 bits fifo */
	if (lcl_size == 32)
	{
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		*param_fiforet = (DD_FEC_FIFO_DATA_32)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		return lcl_err;
	}
	else
	/* if we have native 16 bits fifo */
	{
		lcl_err = glue_internal_read16_onfiforet_send32_toprocess(param_fd, &lcl_word_32);
		*param_fiforet = lcl_word_32;
		return lcl_err;
	}	
	

return -1;

}















DD_TYPE_ERROR glue_fec_get_native_fiforet_item_16(int param_fd, DD_FEC_FIFO_DATA_16 *param_fiforet)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFORET_OFFSET, *param_fiforet};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
	*param_fiforet = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}






//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
/* FIFO RECEIVE BLOCK */
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
//////////////////////////////////////////////////::
	/*!
	Glue functions ; names are explicit.
	*/
	DD_TYPE_ERROR glue_fec_set_frame_in_fiforec_32(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength)
	{
	DD_FEC_FIFO_DATA_32 lcl_frame32[DD_MAX_MSG_LENGTH_32];
	int lcl_length32;
	int lcl_i;
	DD_FEC_ARRAY lcl_fec_offset_and_value;
	DD_TYPE_ERROR lcl_err;

		/* check if we can convert the frame or is it too long ? */
		if (param_stdframelength > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;

		/* convert the frame to 32 bits words */
		dd_cat_from_std_to_32(param_stdframecontent, lcl_frame32, param_stdframelength, &lcl_length32);

		/* Write the frame in fifo */
		for (lcl_i=0; lcl_i<lcl_length32 ; lcl_i++)
		{
			lcl_fec_offset_and_value[DD_FEC_OFFSET_INDEX]= (DD_FEC_UNIFIED_DATA)DD_FEC_FIFOREC_OFFSET;
			lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX]= (DD_FEC_UNIFIED_DATA)lcl_frame32[lcl_i];
			lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
			if (lcl_err != DD_RETURN_OK) return lcl_err;
		}

		return DD_RETURN_OK;
	}


	/*!
	Glue functions ; names are explicit.
	*/
	DD_TYPE_ERROR glue_fec_set_frame_in_fiforec_16(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength)
	{
	DD_FEC_ARRAY lcl_fec_offset_and_value;
	int lcl_i;
	DD_TYPE_ERROR lcl_err;

		for (lcl_i=0; lcl_i<param_stdframelength ; lcl_i++)
		{
			lcl_fec_offset_and_value[DD_FEC_OFFSET_INDEX]= (DD_FEC_UNIFIED_DATA)DD_FEC_FIFOREC_OFFSET;
			lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX]= (DD_FEC_UNIFIED_DATA)param_stdframecontent[lcl_i];
			lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
			if (lcl_err != DD_RETURN_OK) return lcl_err;
		}
		return DD_RETURN_OK;
	}


/*!
Glue functions ; names are explicit.
*/
/*
DD_TYPE_ERROR glue_fec_set_native_fiforec_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fiforec)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, param_fiforec};
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value) );
}
*/


DD_TYPE_ERROR glue_fec_set_native_fiforec_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fiforec)
{
	DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, param_fiforec};
	DD_TYPE_ERROR lcl_err;
	int lcl_size;

	/* this function is called by upper layers, whatever the kind of FEC we have.
	Therefore, if the FEC we drive is REALLY 32 bits then : np, direct write
	else, if if the FEC we drive is NOT 32 bits but IS 16 bits, we will have to fake it */

	/* First, check with which kind of fifo depth we have to deal with */
	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;

	/* if we have native 32 bits fifo */
	if (lcl_size == 32)
	{
		lcl_err = ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value);
		return lcl_err;
	}
	else
	/* if we have native 16 bits fifo */
	{
		lcl_err = glue_internal_read32_fromprocess_write16_tofiforec(param_fd, param_fiforec);
		return lcl_err;
	}	
	

return -1;

}















DD_TYPE_ERROR glue_fec_set_native_fiforec_item_16(int param_fd, DD_FEC_FIFO_DATA_16 param_fiforec)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, param_fiforec};
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_FIFO, &lcl_fec_offset_and_value) );
}





/*!
Glue functions ; names are explicit.
Allow to read a 32 bits word with and old 16 bits capable call.
Note that if a NULL pointer is passed to the function (param_fiforec), a reset of the parameters is done
*/
DD_TYPE_ERROR glue_fec_get_fiforec_item_32(int param_fd, DD_FEC_STD_WORD *param_fiforec)
{
//#define DEBUG_glue_fec_get_fiforec

static int nb_32bits_words_read=0;
static int total_nb_std_words_transmitted=0;
static int partial_nb_std_words_transmitted=0;
static DD_FEC_FIFO_DATA_32 current_32bits_word=0x0;
static int lcl_frame_length;
static DD_FEC_STD_WORD word0, word1, word2, word3;

DD_FEC_STD_WORD lcl_fl1, lcl_fl2;
                                                                                
DD_FEC_STD_WORD status0=0x0;
DD_TYPE_ERROR lcl_err=DD_RETURN_OK;
DD_FEC_ARRAY lcl_fec_offset_and_value_for_status;
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, 0};

// Modified by FD and Guido Magazzu 14/01/2005 => FD 
 if ( param_fiforec == NULL) {

   nb_32bits_words_read=0;
   total_nb_std_words_transmitted=0;
   partial_nb_std_words_transmitted=0;
   current_32bits_word=0x0;

   return DD_RETURN_OK ;
 }
// End of modification 

lcl_fec_offset_and_value[1] = *param_fiforec ;

	#ifdef DEBUG_glue_fec_get_fiforec
		printf("Entering function.\n");
		printf("nb_32bits_words_read=%d, total_nb_std_words_transmitted=%d, partial_nb_std_words_transmitted=%d\n",nb_32bits_words_read,total_nb_std_words_transmitted,partial_nb_std_words_transmitted);
	#endif


	//If we have to process a new 32 bits word
	if (partial_nb_std_words_transmitted==0)
	{

		//first, check if we have something in our fifo receive
		lcl_fec_offset_and_value_for_status[0]=DD_FEC_STAT0_OFFSET;
		lcl_fec_offset_and_value_for_status[1]=status0;
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value_for_status);
		if (lcl_err != DD_RETURN_OK)
		{
			partial_nb_std_words_transmitted = 0 ;
			total_nb_std_words_transmitted = 0;
			nb_32bits_words_read = 0;
			return lcl_err;
		}
		status0 = lcl_fec_offset_and_value_for_status[DD_FEC_VALUE_INDEX];
		#ifdef DEBUG_glue_fec_get_fiforec
			printf("fec status register 0 is : 0x%x\n", status0);
		#endif

		if (status0 & DD_FEC_REC_EMPTY)
		{
			#ifdef DEBUG_glue_fec_get_fiforec
				printf("Fifo receive detected as empty\n");
			#endif
			*param_fiforec =0xFFFF;
			return DD_RETURN_OK;
		}





		#ifdef DEBUG_glue_fec_get_fiforec
			printf("Read 32 bits word requested\n");
		#endif
		//read the 32 bits word
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		if (lcl_err != DD_RETURN_OK)
		{
			partial_nb_std_words_transmitted = 0 ;
			total_nb_std_words_transmitted = 0;
			nb_32bits_words_read = 0;
			return lcl_err;
		}
		current_32bits_word = lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		nb_32bits_words_read++;
		#ifdef DEBUG_glue_fec_get_fiforec
			printf("I have read : 0x%x\n",current_32bits_word);
		#endif

		//If we are reading the first 32 bits word,
		//perform a frame length analysis
		if (nb_32bits_words_read == 1)
		{
			lcl_fl1 = (current_32bits_word >> 8) & 0xFF;
			lcl_fl2 = current_32bits_word & 0xFF;
			#ifdef DEBUG_glue_fec_get_fiforec
				printf("My first length word is : 0x%x\n",lcl_fl1);
				printf("My second length word is : 0x%x\n",lcl_fl2);
			#endif


			if ((lcl_fl1 & 0x0080)==0)
			{
				#ifdef DEBUG_glue_fec_get_fiforec
					printf("I'm in short frame case\n");
				#endif
				lcl_frame_length = lcl_fl1 +3;
			}
			else
			{
				lcl_frame_length = ((((lcl_fl2 & 0x007F) << 8) + lcl_fl1) +4);
				#ifdef DEBUG_glue_fec_get_fiforec
					printf("I'm in long frame case\n");
				#endif
			}
			lcl_frame_length++;
		}

		#ifdef DEBUG_glue_fec_get_fiforec
			printf("I see a lcl_frame_length of %d\n",lcl_frame_length);
		#endif

		//in all cases, if we are processing a new 32 bits word,
		//prepare the 16 bits words to send back to the calling function
		word0 = (current_32bits_word >> 24) & 0xFF;
		word1 = (current_32bits_word >> 16) & 0xFF;
		word2 = (current_32bits_word >> 8) & 0xFF;
		word3 = current_32bits_word & 0xFF;
	}

	//Now, check what we must transmit and how
	//First, if we have no need to play with the status byte
	if (total_nb_std_words_transmitted < (lcl_frame_length-2))
	{
		switch (partial_nb_std_words_transmitted)
		{
			case 0:
				partial_nb_std_words_transmitted =1 ;
				total_nb_std_words_transmitted++;
				*param_fiforec = word0;
				break;

			case 1:
				partial_nb_std_words_transmitted =2 ;
				total_nb_std_words_transmitted++;
				*param_fiforec = word1;
				break;

			case 2:
				partial_nb_std_words_transmitted =3 ;
				total_nb_std_words_transmitted++;
				*param_fiforec = word2;
				break;

			case 3:
				partial_nb_std_words_transmitted =0 ;
				total_nb_std_words_transmitted++;
				*param_fiforec = word3;
				break;
		}
	}
	//Else, if the status byte if needed
	else
	{
		#ifdef DEBUG_glue_fec_get_fiforec
			printf("Now, the status byte must be processed\n");
		#endif
		switch (partial_nb_std_words_transmitted)
		{
			case 0:
				*param_fiforec = (word0 | (word1 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforec
					printf("I am in case 0\n");
				#endif
				break;

			case 1:
				*param_fiforec = (word1 | (word2 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforec
					printf("I am in case 1\n");
				#endif
				break;

			case 2:
				*param_fiforec = (word2 | (word3 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforec
					printf("I am in case 2\n");
				#endif
				break;

			case 3:
				lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
				if (lcl_err != DD_RETURN_OK)
				{
					partial_nb_std_words_transmitted = 0 ;
					total_nb_std_words_transmitted = 0;
					nb_32bits_words_read = 0;
					return lcl_err;
				}
				current_32bits_word = lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
				word0 = (current_32bits_word >> 24) & 0xFF;
				*param_fiforec = (word3 | (word0 << 8));
				partial_nb_std_words_transmitted = 0 ;
				total_nb_std_words_transmitted = 0;
				nb_32bits_words_read = 0;
				#ifdef DEBUG_glue_fec_get_fiforec
					printf("I am in case 3\n");
				#endif
				break;
		}

	}

	#ifdef DEBUG_glue_fec_get_fiforec
		printf("Exiting function.\n");
		printf("nb_32bits_words_read=%d, total_nb_std_words_transmitted=%d, partial_nb_std_words_transmitted=%d\n",nb_32bits_words_read,total_nb_std_words_transmitted,partial_nb_std_words_transmitted);
	#endif

	return lcl_err;
}


	/*!
	Glue functions ; names are explicit.
	*/
	DD_TYPE_ERROR glue_fec_get_fiforec_item_16(int param_fd, DD_FEC_STD_WORD *param_fiforec)
	{
		DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, *param_fiforec};
		DD_TYPE_ERROR lcl_err;
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		*param_fiforec = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		return lcl_err;
	}


DD_TYPE_ERROR glue_fec_get_fiforec_item(int param_fd, DD_FEC_STD_WORD *param_fiforec)
{
int lcl_size;
DD_TYPE_ERROR lcl_err;

	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;
	if (lcl_size == 16)
	{
		lcl_err = glue_fec_get_fiforec_item_16(param_fd, param_fiforec);
	}
	else lcl_err = glue_fec_get_fiforec_item_32(param_fd, param_fiforec);
	return lcl_err;
}

	
	

/*!
See the public file glue.h for this API description.
*/
/*
DD_TYPE_ERROR glue_fec_get_native_fiforec_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fiforec)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, *param_fiforec};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
	*param_fiforec = (DD_FEC_FIFO_DATA_32)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}
*/

DD_TYPE_ERROR glue_fec_get_native_fiforec_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fiforec)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, *param_fiforec};
DD_TYPE_ERROR lcl_err;
int lcl_size;
DD_FEC_FIFO_DATA_32 lcl_word_32;

	/* this function is called by upper layers, whatever the kind of FEC we have.
	Therefore, if the FEC we drive is REALLY 32 bits then : np, direct read
	else, if if the FEC we drive is NOT 32 bits but IS 16 bits, we will have to fake it */

	/* First, check with which kind of fifo depth we have to deal with */
	lcl_err = glue_fec_get_fifo_item_size(param_fd, &lcl_size);
	if (lcl_err != DD_RETURN_OK) return lcl_err;
	if ((lcl_size != 16) && (lcl_size != 32)) return -1;

	/* if we have native 32 bits fifo */
	if (lcl_size == 32)
	{
		lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
		*param_fiforec = (DD_FEC_FIFO_DATA_32)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
		return lcl_err;
	}
	else
	/* if we have native 16 bits fifo */
	{
		lcl_err = glue_internal_read16_onfiforec_send32_toprocess(param_fd, &lcl_word_32);
		*param_fiforec = lcl_word_32;
		return lcl_err;
	}	
	

return -1;

}







DD_TYPE_ERROR glue_fec_get_native_fiforec_item_16(int param_fd, DD_FEC_FIFO_DATA_16 *param_fiforec)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIFOREC_OFFSET, *param_fiforec};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_FIFO, &lcl_fec_offset_and_value);
	*param_fiforec = (DD_FEC_FIFO_DATA_16)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}





/*!
See the public file glue.h for this API description.
*/
/* DEPRECIATED - DO NOT USE ANYMORE.
Rather make use of transaction status sent back after each transaction) */
DD_TYPE_ERROR glue_fec_get_return_status(int param_fd, DD_FEC_STD_WORD *param_status)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_RETURN_STATUS, param_status) );
}



/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_init_ttcrx(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_FEC_INIT_TTCRX) );
}



DD_TYPE_ERROR glue_fec_write_frame(int param_fd, DD_FEC_STD_WORD *param_stack)
{
	return( ioctl(param_fd, DD_IOC_FEC_WRITE_FRAME, param_stack) );
}



DD_TYPE_ERROR glue_fec_read_frame(int param_fd, DD_FEC_STD_WORD *param_stack)
{
	return( ioctl(param_fd, DD_IOC_FEC_READ_FRAME, param_stack) );
}


DD_TYPE_ERROR glue_fec_soft_reset(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_FEC_SOFT_RESET_MODULE) );
}





/* DEPRECIATED - DO NOT USE ANYMORE.
Rather use explicit functions (write to control register 0, etc.) */
DD_TYPE_ERROR glue_fec_write_register(int param_fd, DD_FEC_UNIFIED_DATA *param_array)
{
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, param_array) );
}



DD_TYPE_ERROR glue_fec_write_16(int param_fd, __u16 value, __u16 offset)
{
DD_FEC_ARRAY lcl_array;
	lcl_array[DD_FEC_OFFSET_INDEX] = (DD_FEC_UNIFIED_DATA)offset;
	lcl_array[DD_FEC_VALUE_INDEX] = (DD_FEC_UNIFIED_DATA)value;
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, lcl_array) );
}


DD_TYPE_ERROR glue_fec_write_32(int param_fd, __u32 value, __u32 offset)
{
DD_FEC_ARRAY lcl_array;
	lcl_array[DD_FEC_OFFSET_INDEX] = (DD_FEC_UNIFIED_DATA)offset;
	lcl_array[DD_FEC_VALUE_INDEX] = (DD_FEC_UNIFIED_DATA)value;
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, lcl_array) );
}



/* DEPRECIATED - DO NOT USE ANYMORE.
Rather use explicit functions (Read from control register 0, etc.) */
DD_TYPE_ERROR glue_fec_read_register(int param_fd, DD_FEC_UNIFIED_DATA *param_array)
{
	return( ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, param_array) );
}


DD_TYPE_ERROR glue_fec_read_16(int param_fd, __u16 *value, __u16 offset)
{
DD_FEC_ARRAY lcl_array;

	lcl_array[DD_FEC_OFFSET_INDEX] = (DD_FEC_UNIFIED_DATA)offset;
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, lcl_array) );
}

DD_TYPE_ERROR glue_fec_read_32(int param_fd, __u32 *value, __u32 offset)
{

DD_FEC_ARRAY lcl_array;

	lcl_array[DD_FEC_OFFSET_INDEX] = (DD_FEC_UNIFIED_DATA)offset;
	return( ioctl(param_fd, DD_IOC_WRITE_TO_FEC_REGISTER, lcl_array) );
}


DD_TYPE_ERROR glue_fecdriver_force_count_to_one(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_FEC_FORCE_COUNT_TO_ONE) );
}


DD_TYPE_ERROR glue_fecdriver_get_users_counter(int param_fd, int *param_count)
{
	return( ioctl(param_fd, DD_IOC_FEC_FORCE_COUNT_TO_ONE, param_count) );
}


DD_TYPE_ERROR glue_fecdriver_get_driver_version(int param_fd, char *param_version_string)
{
	return( ioctl(param_fd, DD_IOC_FEC_GET_DRIVER_VERSION, param_version_string) );
}



/*
DD_TYPE_ERROR is actually of type : int
the message stack -msg- must be at most of : XDD_MAX_DECODED_ERROR_MSG_LENGTH = 512 chars long, included terminating \0.
*/
DD_TYPE_ERROR glue_fecdriver_get_error_message(DD_TYPE_ERROR code, char *msg)
{
	return (dd_get_error_msg ( code, msg ));
}


/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR glue_fecdriver_print_error_message(DD_TYPE_ERROR code)
{
	return (dd_print_error_msg ( code ));
}






/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR glue_fec_ring_error_manager(int param_fd, DD_FEC_REM_TYPE* param_cmd)
{
DD_TYPE_ERROR lcl_err;

	lcl_err = ioctl(param_fd, DD_IOC_RING_ERROR_MANAGEMENT, param_cmd);
	return lcl_err;
}


/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR glue_fec_get_plx_reset_counter(int param_fd, DD_RESET_COUNTER_TYPE* param_cmd)
{
DD_TYPE_ERROR lcl_err;

	lcl_err = ioctl(param_fd, DD_IOC_GET_PLX_RESET_COUNTER, param_cmd);
	return lcl_err;
}


/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR glue_fec_get_fec_reset_counter(int param_fd, DD_RESET_COUNTER_TYPE* param_cmd)
{
DD_TYPE_ERROR lcl_err;

	lcl_err = ioctl(param_fd, DD_IOC_GET_FEC_RESET_COUNTER, param_cmd);
	return lcl_err;
}


/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR glue_fec_get_driver_general_status(int param_fd, DD_DRIVER_STATUS_TYPE* param_cmd)
{
DD_TYPE_ERROR lcl_err;

	lcl_err = ioctl(param_fd, DD_IOC_GET_DRIVER_STATUS, param_cmd);
	return lcl_err;
}



/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR  glue_fec_open_device(int *fd, char *device_name, int device_mode)
{
	/*! Ouverture du device et test du resultat */
	(*(fd))=0;
	(*(fd))=open(device_name,device_mode);
	if ((*(fd)) >= 0)
	{
		return DD_RETURN_OK;
	}
	else
	{
		return DD_CANNOT_OPEN_FEC_DRIVER;
	}
}




/*!
See the public file glue.h for this API description.
*/
DD_TYPE_ERROR glue_fec_close_device(int fd)
{
DD_TYPE_ERROR lcl_err;
	lcl_err=close(fd);
	if (lcl_err == DD_RETURN_OK)
	{
		/*! On decremente le nombre d'utilisateurs du device */
		return DD_RETURN_OK;
	}
	else
	{
		return DD_CANNOT_CLOSE_FEC_DRIVER;
	}
}



DD_TYPE_ERROR glue_fec_wait_warning(int param_fd, DD_FEC_STD_WORD *param_stack)
{
	return( ioctl(param_fd, DD_IOC_WAIT_WARNING, param_stack) );
}



DD_TYPE_ERROR glue_fec_simulate_warning(int param_fd)
{
	return( ioctl(param_fd, DD_IOC_SIMULATE_WARNING) );
}



DD_TYPE_ERROR glue_fec_get_fifo_item_size(int param_fd, int *size_in_bits)
{
	return( ioctl(param_fd, DD_IOC_GET_FIFO_ITEM_SIZE, size_in_bits) );
}

/* A returned value equal to 0 means : The driver is associated to an electrical FEC
A returned value equal to 1 means : The driver is associated to an optical FEC */
DD_TYPE_ERROR glue_fec_get_driver_type(int param_fd, int *type)
{
	return( ioctl(param_fd, DD_IOC_GET_FEC_TYPE, type) );
}





/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_firmware_version(int param_fd, DD_FEC_REGISTER_DATA *param_firmware_version)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIRMWARE_VERSION_OFFSET, *param_firmware_version};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_firmware_version = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}




/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_fec_type(int param_fd, int *param_fec_type)
{
	return (ioctl(param_fd, DD_IOC_GET_FEC_TYPE, param_fec_type));
}


/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_vendor_id(int param_fd, unsigned short *param_vendor_id)
{
	return (ioctl(param_fd, DD_IOC_GET_VENDOR_ID, param_vendor_id));
}

/*!
Glue functions ; names are explicit.
*/
DD_TYPE_ERROR glue_fec_get_device_id(int param_fd, unsigned short *param_device_id)
{
	return (ioctl(param_fd, DD_IOC_GET_DEVICE_ID, param_device_id));
}


/*!
Glue functions ; names are explicit.
*/
/*
DD_TYPE_ERROR glue_fec_get_firmware_date(int param_fd, DD_FEC_REGISTER_DATA *param_firmware_date)
{
DD_FEC_ARRAY lcl_fec_offset_and_value = {DD_FEC_FIRMWARE_DATE_OFFSET, *param_firmware_date};
DD_TYPE_ERROR lcl_err;
	lcl_err = ioctl(param_fd, DD_IOC_READ_FROM_FEC_REGISTER, &lcl_fec_offset_and_value);
	*param_firmware_date = (DD_FEC_REGISTER_DATA)lcl_fec_offset_and_value[DD_FEC_VALUE_INDEX];
	return lcl_err;
}
*/


