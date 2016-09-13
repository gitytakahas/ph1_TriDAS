/*
 *      FTDI 245BM chip  USB driver.
 *      Copyright 2004-2005 Wojciech Bialas <Wojciech.Bialas@cern.ch>
 */
/*      BASED HAVELY ON : brlvgr.c by :       
 *      Copyright 2001-2002 Stephane Dalton <sdalton@videotron.ca>
 *                      and Stéphane Doyon  <s.doyon@videotron.ca>
 */
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/* History for brlvgr.c:
 * 0.8 April 2002: Integration into the kernel tree.
 * 0.7 October 2001: First public release as a module, distributed with
 *     the BRLTTY package (beta versions around 2.99y).
 */

#define DRIVER_VERSION "v0.2"
#define DATE "December 2004"
#define DRIVER_AUTHOR "Wojciech BIALAS <Wojciech.Bialas@cern.ch>"
#define DRIVER_DESC "FTDI 245BM PIO mode USB driver for Linux 2.4"
#define DRIVER_SHORTDESC "FTDI245PIO"

#define BANNER \
	KERN_INFO DRIVER_SHORTDESC " " DRIVER_VERSION " (" DATE ")\n" \
	KERN_INFO "   by " DRIVER_AUTHOR "\n"

static const char longbanner[] = {
	DRIVER_DESC ", " DRIVER_VERSION " (" DATE "), by " DRIVER_AUTHOR
};

#include <linux/module.h>
#include <linux/usb.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/poll.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/brlvger.h>

#include "ftdipio.h"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* Module parameters */

static int debug = 0;
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug level, 0-3");



static int ftdipio_major = 0 ; 


/*static int mycontrolmsg(const char *funcname,
                        struct ftdipio_priv *priv, unsigned pipe_dir,
                        __u8 request, __u8 requesttype, __u16 value,
                        __u16 index, void *data, __u16 size);*/

#define controlmsg(priv,pipe_dir,a,b,c,d,e,f) \
     mycontrolmsg(__FUNCTION__, priv, pipe_dir, \
                  a,b,c,d,e,f)
#define sndcontrolmsg(priv,a,b,c,d,e,f) \
    controlmsg(priv, 0, a,b,c,d,e,f)
#define rcvcontrolmsg(priv,a,b,c,d,e,f) \
    controlmsg(priv, USB_DIR_IN, a,b,c,d,e,f)

extern devfs_handle_t usb_devfs_handle; /* /dev/usb dir. */

/* ----------------------------------------------------------------------- */

/* Data */


/* Globals */

/* Table of connected devices, a different minor for each. */
static struct ftdipio_priv *minor_table[ MAX_NR_FTDIPIO_DEVS ];

/* Mutex for the operation of removing a device from minor_table */
static DECLARE_MUTEX(disconnect_sem);

/* For blocking open */
static DECLARE_WAIT_QUEUE_HEAD(open_wait);

/* Driver registration */

#define FTDIPIO_MINOR 192

static struct usb_device_id ftdipio_ids [] = {
	{ USB_DEVICE(0x0403, 0x6006) },
	{ }                     /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, ftdipio_ids);

static struct file_operations ftdipio_fops =
{
	owner:		THIS_MODULE,
	llseek:		ftdipio_llseek,
	read:		ftdipio_read,
	write:		ftdipio_write,
	ioctl:		ftdipio_ioctl,
	open:		ftdipio_open,
	release:	ftdipio_release,
	poll:		ftdipio_poll,
};

static struct usb_driver ftdipio_driver =
{
	name:		"ftdipio",
	probe:		ftdipio_probe,
	disconnect:	ftdipio_disconnect,
	fops:		&ftdipio_fops,
	minor:		FTDIPIO_MINOR,
	id_table:	ftdipio_ids,
};

static int
__init ftdipio_init (void)
{

   int result = 0 ; 

	printk(BANNER);

/* 	if(stall_tries < 1 || write_repeats < 1) */
/* 	  return -EINVAL; */

	memset(minor_table, 0, sizeof(minor_table));


	result=devfs_register_chrdev(ftdipio_major,"ftdipio",&ftdipio_fops); 

	if (result<0) { 
	   err("Character device registration failed");
	   return -ENOSYS; 
	}

	if (ftdipio_major>0) { 
	   if (result!=0) { 
	   err("Character device registration failed");
	   return -ENOSYS;	      
	   }    
	} else { 
	   if (result>0) ftdipio_major = result ; 
	} 


	


	if (usb_register(&ftdipio_driver)) {
		err("USB registration failed");
		return -ENOSYS;
	}

	dbg("Driver registered");
	

	return 0;
}

static void
__exit ftdipio_cleanup (void)
{

   int result = 0 ; 

   result = devfs_unregister_chrdev (ftdipio_major,"ftdipio");
   if (!result) { 
      err("Problems with unregistration of ftdipio character device");
   } 
   dbg("Character device unregistered");

   

   

	usb_deregister (&ftdipio_driver);
	dbg("USB Driver unregistered");
}

module_init (ftdipio_init);
module_exit (ftdipio_cleanup);


/**
 *	ftdipio_delete
 */
static inline void ftdipio_delete (struct ftdipio_priv *dev)
{

  if (dev ) { 
    dbg2("ftdipio_delete: deleting private data");
	minor_table[dev->subminor] = NULL;

	if (dev->out_urb) usb_free_urb(dev->out_urb) ;
	

	if (dev->in_urb) usb_free_urb(dev->in_urb) ; 

       
	if (dev->out_buf_urb != NULL)
		kfree (dev->out_buf_urb);

	if (dev->in_buf_urb != NULL)
		kfree (dev->in_buf_urb);

	if (dev->write_buffer != NULL)
		kfree (dev->write_buffer);

	if (dev->read_buffer != NULL)
		kfree (dev->read_buffer);
 	byte_fifo_destroy(&dev->write_fifo);
 	byte_fifo_destroy(&dev->read_fifo);  
	kfree (dev);
	dev = NULL ;
  }
  dbg2("ftdipio_delete: done");
 
}

static inline int ftdipio_is_dev_there(struct ftdipio_priv *priv)
{
  int ret = -1 ; /* by default no device there */ 
  if (priv) { 
    /*down(&priv->dev_sem);*/
    ret = (atomic_read(&priv->dev_connected)) ? 0 : -1 ;
    /*up(&priv->dev_sem);*/
    return ret ; 
    
  } 
  return ret ; 
}
static int ftdipio_to_read(struct ftdipio_priv *priv)
{
  unsigned long int flags ; size_t cnt = 0 ;   
  spin_lock_irqsave( &priv->in_lock, flags );
  cnt = byte_fifo_get_actual_size(&priv->read_fifo);
  spin_unlock_irqrestore( &priv->in_lock, flags );
  return cnt ; 
} 


/* ----------------------------------------------------------------------- */

/* Probe and disconnect functions */

static void *
ftdipio_probe (struct usb_device *dev, unsigned ifnum,
	       const struct usb_device_id *id)
{
	struct ftdipio_priv *priv = NULL;
	int i;
	struct usb_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_interface_descriptor *iface_desc;
	int buffer_size ; 
	/* protects against reentrance: once we've found a free slot
	   we reserve it.*/
	static DECLARE_MUTEX(reserve_sem);
        char devfs_name[16];
	int found_in,found_out ; 

	 

/* 	actifsettings = dev->actconfig->interface->altsetting; */

/* 	if( dev->descriptor.bNumConfigurations != 1 */
/* 			|| dev->config->bNumInterfaces != 1  */
/* 			|| actifsettings->bNumEndpoints != 1 ) { */
/* 		err ("Bogus ftdi pio config info"); */
/* 		return NULL; */
/* 	} */


/* 	endpoint = actifsettings->endpoint; */
/* 	if (!(endpoint->bEndpointAddress & 0x80) || */
/* 		((endpoint->bmAttributes & 3) != 0x02)) { */
/* 		err ("Bogus ftdi pio config info, wrong endpoints"); */
/* 		return NULL; */
/* 	} */

	down(&reserve_sem);  
	

	for( i = 0; i < MAX_NR_FTDIPIO_DEVS; i++ )
		if( minor_table[i] == NULL )
			break;

	if( i == MAX_NR_FTDIPIO_DEVS ) {
		err( "This driver cannot handle more than %d "
				"FTDI 245BM chips", MAX_NR_FTDIPIO_DEVS);
		goto error;
	}

	if( !(priv = kmalloc (sizeof *priv, GFP_KERNEL)) ){
		err("No more memory");
		goto error;
	}

	dbg2("priv object allocation done.") ; 

	memset(priv, 0, sizeof(*priv));

	dbg2("memset done");


	priv->in_urb = NULL ; 
	priv->out_urb = NULL ; 

	dbg2("allocating read/write fifos buffers") ; 

	if(!byte_fifo_create(&priv->read_fifo,MAX_FTDIPIO_READ_BUF)) { 
	  err("cannot allocate memory for read_fifo buffer");
	  goto error ; 
	  };

	if(!byte_fifo_create(&priv->write_fifo,MAX_FTDIPIO_WRITE_BUF)) { 
	  err("cannot allocate memory for write_fifo buffer");
	  goto error ; 
	  };
	if( !(priv->read_buffer = kmalloc (MAX_FTDIPIO_READ_BUF, GFP_KERNEL)) ){
		err("read_buffer: No more memory");
		goto error;
	}
	if( !(priv->write_buffer = kmalloc (MAX_FTDIPIO_WRITE_BUF, GFP_KERNEL)) ){
		err("write_buffer: No more memory");
		goto error;
	}	

	
	dbg2("allocating read/write buffers done.");

	spin_lock_init(&priv->in_lock);
	spin_lock_init(&priv->out_lock);
	init_waitqueue_head(&priv->read_wait);
	init_waitqueue_head(&priv->write_wait);
	/* opened is memset'ed to 0 */
	init_MUTEX(&priv->open_sem);
	init_MUTEX(&priv->dev_sem);
	init_MUTEX(&priv->gen_sem);
	atomic_set(&priv->in_urb_timeout, MAX_IN_URB_TIMEOUT);
	atomic_set(&priv->out_urb_timeout, MAX_OUT_URB_TIMEOUT);
	atomic_set(&priv->in_urb_problem,MAX_IN_URB_PROBLEM );
	atomic_set(&priv->out_urb_problem, 0);
	atomic_set(&priv->dev_connected,1);
	atomic_set(&priv->in_to_read,0) ; 
	priv->subminor = i;

	dbg2("spinlocks and mutex initialized");

	/* we found a interrupt in endpoint */


	priv->dev = dev;
	/* in,out bulk endpoints test and memory allocation*/
	
	found_in = 0 ; found_out = 0 ; 

	dbg2("scanning for in/out bulk endpoints...");

	interface = &dev->actconfig->interface[ifnum];
	iface_desc = &interface->altsetting[0];
	for (i = 0; i < iface_desc->bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i];

		

		/* here place the hardware detection & configuration stuff */
		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
		  /* we found a bulk in endpoint */
		  priv->in_urb = NULL ; 
		  priv->in_ep = endpoint ; 
		  /* buffer_size = endpoint->wMaxPacketSize; */
		  buffer_size = MAX_FTDIPIO_URB_BUF ; 
		  dbg2("Max packect size for in bulk is %d",buffer_size);
		  priv->in_size_urb = buffer_size;
		  priv->in_endpointAddr = endpoint->bEndpointAddress;
		  priv->in_buf_urb = kmalloc (buffer_size, GFP_KERNEL);
		  if (priv->in_buf_urb==NULL) {
		    err("Couldn't allocate in_buf_urb");
		    goto error;
		  }
		  if (priv->in_urb==NULL) { 
		    /* Setup usb bulk urb to poll data from the device */
		    priv->in_urb = usb_alloc_urb(0);
		    if(!priv->in_urb) {
		      err("Unable to allocate input URB");
		      goto error;
		    }
		  }


		  found_in = 1 ; 
		} 
		
		if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
		  /* we found a bulk out endpoint */
		  priv->out_urb = NULL ; 
		  priv->out_ep = endpoint ; 
		  /* buffer_size = endpoint->wMaxPacketSize; */
		  buffer_size = MAX_FTDIPIO_URB_BUF ;
		  dbg2("Max packect size for out bulk is %d",buffer_size);
		  priv->out_size_urb = buffer_size;
		  priv->out_endpointAddr = endpoint->bEndpointAddress;
		  priv->out_buf_urb = kmalloc (buffer_size, GFP_KERNEL);
		  if (priv->out_buf_urb==NULL) {
		    err("Couldn't allocate out_buf_urb");
		    goto error;
		  }
		  if (priv->out_urb==NULL) { 
		    priv->out_urb = usb_alloc_urb(0);
		    if(!priv->out_urb) {
		      err("Unable to allocate output URB");
		      goto error;
		    }
		  }
		  found_out = 1 ; 
		} 
		/* 	else {  */
		/* 	  err("No out bulk endpoint found");  */
		/* 	  goto error ;  */
		/* 	}  */

	}

	if (found_in==0) { 
	  err("Couldn't find in bulk endpoint");
	  goto error ; 
	} 
	if (found_out==0) { 
	  err("Couldn't find out bulk endpoint");
	  goto error ; 
	}
	
	dbg2("Scanning successful."); 

	sprintf(devfs_name, "ftdipio%d", priv->subminor);
	priv->devfs = devfs_register(usb_devfs_handle, devfs_name,
				     DEVFS_FL_AUTO_DEVNUM, 0,0,
				     S_IFCHR |S_IRUSR|S_IWUSR |S_IRGRP|S_IWGRP,
				     &ftdipio_fops, NULL);
	if (!priv->devfs) {
#ifdef CONFIG_DEVFS_FS
		err("devfs node registration failed");
#endif
		info("devfs node registration failed");
	}


	minor_table[priv->subminor] = priv;

	info( "FTDI 245 BM - %d is device major %d minor %d",
				priv->subminor, ftdipio_major, FTDIPIO_MINOR + priv->subminor);

	/* Tell anyone waiting on a blocking open */
	wake_up_interruptible(&open_wait);

	goto out;

 error:
	ftdipio_delete(priv) ; 

 out:
	up(&reserve_sem);
	return priv;
}

static void
ftdipio_disconnect(struct usb_device *dev, void *ptr)
{
	struct ftdipio_priv *priv = (struct ftdipio_priv *)ptr;
	int r;

       

	if(priv){

		info("chip minor %d disconnecting", priv->subminor);
		
		
		
		down(&disconnect_sem);
		minor_table[priv->subminor] = NULL;
		up(&disconnect_sem);

		dbg2("private removed from minor table");

		down(&priv->open_sem);
		down(&priv->dev_sem);
		devfs_unregister(priv->devfs);
		dbg2("devfs unregistered"); 
		if(priv->opened) {
			/* Disable interrupts */
			if((r = usb_unlink_urb(priv->in_urb)) <0)
				err("input usb_unlink_urb returns %d", r);
			if((r = usb_unlink_urb(priv->out_urb)) <0)
				err("output usb_unlink_urb returns %d", r);
			dbg2("urbs unlinked");
/* 			if (priv->out_urb!=NULL) {  */
/* 			  usb_free_urb(priv->out_urb); */
/* 			  priv->out_urb = NULL ; */
/* 			} */
/* 			if (priv->in_urb!=NULL) { */
/* 			  usb_free_urb(priv->in_urb); */
/* 			  priv->in_urb = NULL ; */
/* 			} */
/* 		        dbg2("urbs destroyed. Pointers NULLed"); */
			/* mark device as dead and prevent control
			   messages to it */
			priv->dev = NULL;
			atomic_set(&priv->dev_connected,0);

			dbg2("dev pointer zero.");
			/* Tell anyone hung up on a read that it
			   won't be coming */
			/*wake_up_interruptible(&priv->read_wait);*/
			up(&priv->dev_sem);
			up(&priv->open_sem);
		}else
			/* no corresponding up()s */
		  ftdipio_delete(priv) ; 
		dbg2("disconnect done");
	}
}

/* ----------------------------------------------------------------------- */

/* fops implementation */

static int
ftdipio_open(struct inode *inode, struct file *file)
{
	int devnum = MINOR (inode->i_rdev);
	struct ftdipio_priv *priv;
	int n, ret;

	if (devnum < FTDIPIO_MINOR
	    || devnum >= (FTDIPIO_MINOR + MAX_NR_FTDIPIO_DEVS))
		return -ENXIO;

	n = devnum - FTDIPIO_MINOR;

	dbg3("subminor is %d ",n);

	MOD_INC_USE_COUNT;

	do {
		down(&disconnect_sem);
		priv = minor_table[n];
		
		if(!priv) {
			up(&disconnect_sem);
			
			if (file->f_flags & O_NONBLOCK) {
				dbg2("Failing non-blocking open: "
				     "device %d not connected", n);
				MOD_DEC_USE_COUNT;
				return -EAGAIN;
			}
			/* Blocking open. One global wait queue will
			   suffice. We wait until a device for the selected
			   minor is connected. */
			dbg2("Waiting for device %d to be connected", n);
			ret = wait_event_interruptible(open_wait,
						       minor_table[n]
						       != NULL);
			if(ret) {
				dbg2("Interrupted wait for device %d", n);
				MOD_DEC_USE_COUNT;
				return ret;
			}
		}
	} while(!priv);
	/* We grabbed an existing device. */
	dbg3("found existing device");
	if(down_interruptible(&priv->open_sem)) {
	        up(&disconnect_sem);
		return -ERESTARTSYS;
	}
	
	up(&disconnect_sem);

	/* lock device */
	if(down_interruptible(&priv->dev_sem)) {
	  up(&priv->open_sem);
	  return -ERESTARTSYS; 
	} 

	/* Only one process can open each device, no sharing. */
	ret = -EBUSY;
	if(priv->opened)
		goto error;

	dbg("Opening ftdi device  %d", priv->subminor);

	atomic_set(&priv->in_urb_timeout, MAX_IN_URB_TIMEOUT);
	atomic_set(&priv->out_urb_timeout, MAX_OUT_URB_TIMEOUT);
	atomic_set(&priv->in_urb_problem,MAX_IN_URB_PROBLEM );

	dbg2("atomic variables initialized.");

	usb_fill_bulk_urb( priv->in_urb, priv->dev,
			usb_rcvbulkpipe(priv->dev,
				       priv->in_ep->bEndpointAddress),
			priv->in_buf_urb,priv->in_size_urb,
			intr_callback, priv);
	priv->in_urb->actual_length = 0 ; 
	if((ret = usb_submit_urb(priv->in_urb)) <0){
		err("Error %d while submitting input URB", ret);
		goto error;
	}

	

	usb_fill_bulk_urb( priv->out_urb, priv->dev,
			usb_rcvbulkpipe(priv->dev,
				       priv->out_ep->bEndpointAddress),
			priv->out_buf_urb,priv->out_size_urb,
			intr_callback, priv);
/* 	if((ret = usb_submit_urb(priv->in_urb)) <0){ */
/* 		err("Error %d while submitting input URB", ret); */
/* 		goto error; */
/* 	} */

	/* Mark as opened, so disconnect cannot free priv. */

	dbg2("urbs allocated");

	priv->opened = 1;

	file->private_data = priv;

	ret = 0;

	dbg2("ftdipio_open done.");

	goto out;

 error:
	MOD_DEC_USE_COUNT;
 out:
	up(&priv->dev_sem);
	up(&priv->open_sem);
	return ret;
}

static int
ftdipio_release(struct inode *inode, struct file *file)
{
	struct ftdipio_priv *priv = file->private_data;
	int r;

	/* mutex with disconnect and with open */
	if (down_interruptible(&priv->open_sem)) 
	  return -ERESTARTSYS;
	if (down_interruptible(&priv->dev_sem)) {
	  up(&priv->open_sem); 
	  return -ERESTARTSYS;
        } 

	if(!priv->dev) {
		dbg("Releasing disconnected device %d", priv->subminor);
		/* no up(&priv->open_sem) and no down(&priv->dev_sem);*/
		ftdipio_delete(priv);
	}else{
		dbg("Closing device %d", priv->subminor);
		/* Disable interrupts */
		if((r = usb_unlink_urb(priv->in_urb)) <0)
			err("release: in: usb_unlink_urb returns %d", r);
		if((r = usb_unlink_urb(priv->out_urb)) <0)
			err("release: out: usb_unlink_urb returns %d", r);
		dbg2("urbs unlinked");

/* 		usb_free_urb(priv->out_urb); */
/* 		priv->out_urb = NULL ;  */
/* 		usb_free_urb(priv->in_urb); */
/* 		priv->in_urb = NULL ; */
/* 		dbg2("urbs destroyed. Pointers NULLed"); */
		
		/* wake up all processes that were waiting on read/write/ioctl */
		priv->opened = 0;
		wake_up_interruptible(&priv->read_wait);
		wake_up_interruptible(&priv->write_wait);
		
		dbg2("read and write queues waked up");

		up(&priv->dev_sem);
		up(&priv->open_sem);
	}

	MOD_DEC_USE_COUNT;

	dbg("Release done.");

	return 0;
}

static ssize_t
ftdipio_write(struct file *file, const char *buffer,
	      size_t count, loff_t *pos)
{
	struct ftdipio_priv *priv = file->private_data;
	
	/*int ret;*/
	/*size_t rs;*/
	/*loff_t off;*/

	
        unsigned long copy_size;
        unsigned long bytes_written = 0;
        unsigned int partial;
 
        int result = 0;
        int maxretry;
        int errn = 0;

	unsigned char *obuf; 

	if(down_interruptible(&priv->dev_sem))
	  return -ERESTARTSYS;

/* 	if(count > MAX_FTDIPIO_WRITE_BUF) {  */
/* 	  up(&priv->dev_sem); */
/* 	  return -EINVAL; */
/* 	} */

	if ( pos != &file->f_pos ) {
	  up(&priv->dev_sem);
		return -ESPIPE;
	}
	if ( !access_ok(VERIFY_WRITE, buffer, count) ) {
	  up(&priv->dev_sem);
	  return -EFAULT;
	}


        
        /* Sanity check to make sure ftdi245bm is connected, powered, etc */
        if ( priv == NULL ||
             atomic_read(&priv->dev_connected) == 0 ||
             priv->dev == NULL ){
                	up(&priv->dev_sem);
			return -ENODEV;
        }
 
	obuf = priv->write_buffer;
 
        do {
                unsigned long thistime;
		 
                
		int max_bulk_write_size = MAX_FTDIPIO_WRITE_BUF ;

		

                thistime = copy_size =
                    (count >= max_bulk_write_size ) ? max_bulk_write_size : count;

		dbg2("in do loop: count: %d this_time: %lu copy_size: %lu ",count,
		     thistime,copy_size);

		if (obuf==NULL) {
		  dbg2("obuf: is NULL");
		}

                if (copy_from_user(obuf, buffer, copy_size)) {
                        errn = -EFAULT;
                        goto error;
                }
                maxretry = 5;
                while (thistime) {
                        if (priv->dev==NULL) {
                                errn = -ENODEV;
                                goto error;
                        }
                        if (signal_pending(current)) {
                                up(&priv->dev_sem);
                                return bytes_written ? bytes_written : -EINTR;
                        }
 
                        result = usb_bulk_msg(priv->dev,
                                         usb_sndbulkpipe(priv->dev, priv->out_ep->bEndpointAddress),
                                         obuf, thistime, &partial, HZ/20);
 
                        dbg2("write stats: result:%d thistime:%lu partial:%u",
                             result, thistime, partial);
 
                        if (result == USB_ST_TIMEOUT) { /* NAK - so hold for a while */
			  int r ;
			  if ( file->f_flags & O_NONBLOCK ) {
			    up(&priv->dev_sem);
			    return (bytes_written ? bytes_written : -EAGAIN);
			  }
			  

                                if (!maxretry--) {
                                        errn = -ETIME;
                                        goto error;
                                }
				up(&priv->dev_sem);
                                r = interruptible_sleep_on_timeout(&priv->write_wait, NAK_TIMEOUT);
				if (r) return (bytes_written ? bytes_written :  -EINTR ) ;
				if(down_interruptible(&priv->dev_sem))
				  return (bytes_written ? bytes_written : -ERESTARTSYS);
                                continue;
                        } else if (!result & partial) {
                                obuf += partial;
                                thistime -= partial;
                        } else
                                break;
                };
                if (result) {
                        err("Write Whoops - %x", result);
                        errn = -EIO;
                        goto error;
                }
                bytes_written += copy_size;
                count -= copy_size;
                buffer += copy_size;
        } while (count > 0);
 
        up(&priv->dev_sem);
 
        return bytes_written ? bytes_written : -EIO;
 
error:
        
	up(&priv->dev_sem);


	return errn; 
}


/* static int */
/* read_index(struct ftdipio_priv *priv) */
/* { */
/* 	int intr_idx, read_idx; */

/* 	read_idx = atomic_read(&priv->read_idx); */
/* 	read_idx = ++read_idx == MAX_INTERRUPT_BUFFER ? 0 : read_idx; */

/* 	intr_idx = atomic_read(&priv->intr_idx); */

/* 	return(read_idx == intr_idx ? -1 : read_idx); */

/* 	return 0 ;  */

/* } */

static ssize_t
ftdipio_read(struct file *file, char *buffer,
	     size_t count, loff_t *unused_pos)
{
	struct ftdipio_priv *priv = file->private_data;
	int dev_status ; int ret = 0; int index = 0 ;  

	/* lock up the device */

	if(down_interruptible(&priv->dev_sem)) 
	  return -ERESTARTSYS; 


	if(count > MAX_FTDIPIO_READ_BUF) { 
	  up(&priv->dev_sem);
		return -EINVAL;
	}

	if ( unused_pos != &file->f_pos ) {
	  up(&priv->dev_sem);
		return -ESPIPE;
	}
	if ( !access_ok(VERIFY_READ, buffer, count) ) {
	  up(&priv->dev_sem);	
	  return -EFAULT;
	}

/* 	dev_status = ftdipio_is_dev_there(priv); */

/* 	if(dev_status)  { */
/* 		up(&priv->dev_sem); */
/* 			  return -ENOLINK; */
/* 	} */

	index = 0 ; 


	while( count > 0 ) {
		int cnt=0;int cnt2 =0  ;
		int d = (int)count;
		
		  if (signal_pending(current)) {
                        up(&(priv->dev_sem));
                        return ret ? ret : -EINTR;
                  }
		  
		  dev_status = ftdipio_is_dev_there(priv);

		  if(dev_status)  {
			  up(&priv->dev_sem);
			  return (ret ? ret : -ENODEV);
		  }

		  if(!atomic_read(&priv->in_urb_timeout)) {
		    up(&priv->dev_sem);
		    return (ret ? ret : -ETIMEDOUT);
		  } 
		  if(!atomic_read(&priv->in_urb_problem)) {
		     up(&priv->dev_sem);
		     return (ret ? ret : -EIO);
		  }

		/* how many bytes do we have in recv. fifo ? */
		{
			
			unsigned long flags; /* used to synchronize access to the endpoint */
			spin_lock_irqsave( &priv->in_lock, flags );
			cnt = byte_fifo_get_actual_size(&priv->read_fifo);
			spin_unlock_irqrestore( &priv->in_lock, flags );

			dbg3("ftdio_pio_read: detected fifo size: %d \n",cnt);
		}


		
		
		if ( cnt > d ) {
			cnt = d;
		}

		if ( cnt <= 0 ) {
		/*   int r ; int read_cnt ;  */



		  	dev_status = ftdipio_is_dev_there(priv);

			if(dev_status)  {
			  up(&priv->dev_sem);
			  return (ret ? ret : -ENOLINK);
			}
			if ( file->f_flags & O_NONBLOCK ) {
			  up(&priv->dev_sem);
			  return (ret ? ret : -EAGAIN);
			}

			
			/* release the device before we wait ... */

			up(&priv->dev_sem);
			dbg2("ftdipio_read: goiny to sleep");
/* 			r = wait_event_interruptible(priv->read_wait, */
/* 			       (!(dev_status = ftdipio_is_dev_there(priv)) || (read_cnt = ftdipio_to_read(priv)))); */
			interruptible_sleep_on(&priv->read_wait);
			dbg2("ftdi_read: waked up from sleep...");
			if (signal_pending(current)) {
			  return ret ? ret : -EINTR;
			}

			/* handle problems with sleep...*/
			/* if(r) { err("ftdipio_read:interruptible_sleep_on has returned %d",r) ;   return r; } ; */
			
			/* lock the device again: */




			if(down_interruptible(&priv->dev_sem)) 
			  return ret ? ret : -ERESTARTSYS;

			dbg2("ftdipio: got dev semaphore back, index %d ",index); 

			index++; 

			continue; 
		}

		{
			int i;
			unsigned long flags; /* used to synchronize access to the endpoint */
			 
			dbg3("ftdio_pio_read: req fifo read: %d \n",cnt);
			spin_lock_irqsave( &priv->in_lock, flags );
			cnt2 = byte_fifo_get(priv->read_buffer,&priv->read_fifo,cnt);
			spin_unlock_irqrestore( &priv->in_lock, flags );
			dbg3("ftdio_pio_read: fifo read done: %d \n",cnt2);
			copy_to_user(buffer,priv->read_buffer,cnt2);
			for(i=0;i<cnt2;i++) dbg3("ftdio_pio_read: data[%3d] = %2X ",i,(int)(*(buffer+i)));
		}

		count-=cnt2;
		buffer+=cnt2;
		ret+=cnt2;

		
	}

	

/* 	if((read_idx = read_index(priv)) == -1) { */
/* 		/\* queue empty *\/ */
/* 		if (file->f_flags & O_NONBLOCK) */
/* 			return -EAGAIN; */
/* 		else{ */
/* 			int r = wait_event_interruptible(priv->read_wait, */
/* 							 (!priv->dev || (read_idx = read_index(priv)) != 0)); */
/* 			if(!priv->dev) */
/* 				return -ENOLINK; */
/* 			if(r) */
/* 				return r; */
/* 			if(read_idx == -1) */
/* 				/\* should not happen *\/ */
/* 				return 0; */
/* 		} */
/* 	} */

/* 	if (copy_to_user (buffer, priv->inbuffer, count) ) */
/* 		return( -EFAULT); */

	/* atomic_set(&priv->read_idx, read_idx); */
	/* Multiple opens are not allowed. Yet on SMP, two processes could
	   read at the same time (on a shared file descriptor); then it is not
	   deterministic whether or not they will get duplicates of a key
	   event. */
	up(&priv->dev_sem);
	return ret;
}

static int
ftdipio_ioctl(struct inode *inode, struct file *file,
	      unsigned cmd, unsigned long arg)
{
	struct ftdipio_priv *priv = file->private_data;
	int ret = 0 ; 
	int err = 0 ; 
	int usb_result = 0 ;
	void *data; 

	/* check sanity of cmd argument and memory access */ 
	if (_IOC_TYPE(cmd) != FTDIPIO_IOC_MAGIC) { 
	   dbg("not a valid command.");
	   return -ENOTTY;
	}
	if (_IOC_NR(cmd) > FTDIPIO_IOC_MAXNR) { 
	   dbg("command number too high");
	   return -ENOTTY;
	}
	if (_IOC_DIR(cmd) & _IOC_READ) 
	   err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
	   err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	if (err) { 
	   dbg("problems in accessing buffers , cmd %d err %d",cmd,err);
	   return -EFAULT;
	}

	/* sanity check */ 

	if(priv == NULL) return -ENODEV ; 

 

	/* check if we can get access to hardware */ 

	if(down_interruptible(&priv->dev_sem)) 
	  return -ERESTARTSYS;

	if(!priv->dev) { 
	  up(&priv->dev_sem);
		return -ENOLINK;
	}

	/* check if device is still there */

	if(atomic_read(&priv->dev_connected) == 0) { 
	   up(&priv->dev_sem);
	   return -ENODEV ;
	}

	/* finaly do processing */ 
	
	switch(cmd) {
	case FTDIPIO_IOC_RESET : 
	   usb_result = usb_control_msg(priv->dev,
			   usb_sndctrlpipe(priv->dev, 0),
			   0,0x40,0,0, NULL,0,HZ/20); 
	   
	   if (usb_result) { /* if result not ZERO : there was a problem */
	      dbg("problems in device reset , error %d\n",usb_result);
	      ret = -ENOTTY ; 
	   } ; break ; 

	case FTDIPIO_IOC_PURGE_BUFFERS : 
	   usb_result = usb_control_msg(priv->dev,
			   usb_sndctrlpipe(priv->dev, 0),
			   0,0x40,1,0, NULL,0,HZ/20);
	   if (usb_result) { /* if result not ZERO : there was a problem */
	      dbg("problems purge (1) , error %d\n",usb_result);
	      ret = -ENOTTY ; 
	   } ; 
	   usb_result = usb_control_msg(priv->dev,
			   usb_sndctrlpipe(priv->dev, 0),
			   0,0x40,2,0, NULL,0,HZ/20);
	  
	   if (usb_result) { /* if result not ZERO : there was a problem */
	      dbg("problems purge (2) , error %d\n",usb_result);
	      ret = -ENOTTY ; 
	   } ;

	   break ; 

	case FTDIPIO_IOC_SET_LATENCY_TIMER: 
	   data = (void *) arg;
	   copy_from_user(&(priv->latency), data, sizeof(unsigned char));
	   if (priv->latency<1) { ret = -EFAULT; break ; } /* 1-255 only */
	   usb_result = usb_control_msg(priv->dev,
			   usb_sndctrlpipe(priv->dev, 0),
			   0x09,0x40,priv->latency,0, NULL,0,HZ/20);
	  
	   if (usb_result) { /* if result not ZERO : there was a problem */
	      dbg("problems in setting latency , error %d\n",usb_result);
	      ret = -ENOTTY ; 
	   } ;
	   
	      


	   break ; 

	case FTDIPIO_IOC_GET_LATENCY_TIMER: 
	   data = (void *) arg;
	   usb_result = usb_control_msg(priv->dev,
					usb_rcvctrlpipe(priv->dev, 0),
					0xC0,0x0A,0,0,&priv->latency,1,HZ/20);
	  
	   if (usb_result) { /* if result not ZERO : there was a problem */
	      dbg("problems in reading latency , error %d\n",usb_result);
	      ret = -ENOTTY ; 
	   } else { 
	   copy_to_user(data, &(priv->latency),sizeof(unsigned char));
	   }
	   break ; 


	default:
	/* overkill: already checked above */
	   dbg("unknown ioctl command , cmd %d\n",cmd);
	   

		ret =  -ENOTTY;
	};


	up(&priv->dev_sem);
	return ret ; 
}

static loff_t
ftdipio_llseek(struct file *file, loff_t offset, int orig)
{
  return -ESPIPE;
}


static unsigned
ftdipio_poll(struct file *file, poll_table *wait) 
{
	struct ftdipio_priv *priv = file->private_data;
/* 	int dev_status = 0 ; */
	int ret = 0 ; int to_read = 0 ; 

/* 	down(&priv->dev_sem);  */

/* 	dev_status = ftdipio_is_dev_there(priv); */

/* 	if(dev_status) {  */
/* 		ret =  POLLERR | POLLHUP; */
/* 		goto end ;  */
/* 	} */

/* 	up(&priv->dev_sem); */

/* 	poll_wait(file, &priv->read_wait, wait); */

/* 	down(&priv->dev_sem); */

/* 	dev_status = ftdipio_is_dev_there(priv); */

/* 	if(dev_status) {  */
/* 		ret = POLLERR | POLLHUP; */
/* 	} */
/* 	if(ftdipio_to_read(priv)) {  */
/* 		ret = POLLIN | POLLRDNORM; */
/* 	} */
/*  end:  */
/* 	up(&priv->dev_sem); */

	if(!atomic_read(&priv->dev_connected)) {
	  dbg2("ftdipio_poll: (1) device disconnected");
	  ret =  POLLERR | POLLHUP;
	  return ret ;
	}
	dbg2("ftdipio_poll: goining into poll_wait...");
	poll_wait(file, &priv->read_wait, wait);

	dbg2("ftdipio_poll: WAKED UP from  poll_wait...");

	if(!atomic_read(&priv->dev_connected)) {
	  ret =  POLLERR | POLLHUP;
	  dbg2("ftdipio_poll: (2) device disconnected");
	  return ret ;
	}
	/* down(&priv->dev_sem); */
	if((to_read=ftdipio_to_read(priv))) {
 		ret = POLLIN | POLLRDNORM;
		dbg2("ftdipio_poll: %d bytes to read",to_read);
 	}
/* 	/\* up(&priv->dev_sem); *\/ */
	

	return ret;
}


static void
intr_callback(struct urb *urb)
{
	struct ftdipio_priv *priv = urb->context;
/* 	int intr_idx, read_idx; */
	int i ; 
	int ret ; int wake ;  
	unsigned long flags ;
/* 	int dev_status ; */ 
	int pushed_fifo_size ; 

	unsigned char *data = urb->transfer_buffer ; 

	/*dbg2("read callback called");*/


	if( urb->status ) {
	  if(urb->status == -ETIMEDOUT) { 
			dbg("intr_callback: in urb Status -ETIMEDOUT, "
			     "probably disconnected");
			if(atomic_dec_and_test(&priv->in_urb_timeout)) {
			  wake_up_interruptible(&priv->read_wait);
			  return ; 
			} 
	  }
	  else { 
	    if(urb->status != -ENOENT) err("intr_callback: in urb Status: %d", urb->status);
	    /* problem with usb reading wake up all readers */
	    dbg3("urb : actlen: %d buflen: %d ",urb->actual_length,urb->transfer_buffer_length);
	    if(atomic_dec_and_test(&priv->in_urb_problem)) {
			  wake_up_interruptible(&priv->read_wait);
			  dbg2("problems with urb in callback, waking up wait_read, done");
			  return ; 
	    } else {  /* unlink called */
	      atomic_set(&priv->in_urb_problem,0); /* signal to readers that usb polling token is down*/
	      wake_up_interruptible(&priv->read_wait);	      
	      return ;
	    }
	  }

	} else { 
	  atomic_set(&priv->in_urb_timeout, MAX_IN_URB_TIMEOUT);
	  atomic_set(&priv->in_urb_timeout, MAX_IN_URB_PROBLEM);
	  if (urb->actual_length!=2) { 
	  dbg3("Data: size %d ",urb->actual_length);
	  for(i=0;i<urb->actual_length;i++) 
	    dbg3(" %02X",((int)data[i]));
	    dbg3("\n");
	  } else { 
	    if ((data[0] != 0x31) || (data[1] != 0x60))
	      dbg3("strange chip status: 0x%02X 0x%02X",(int)data[0],(int)data[1]); 
	  } 
	  wake = 0;
	  
	  
	  if (urb->actual_length>2) {

	    spin_lock_irqsave( &priv->in_lock, flags );

		/* my stuff to get data from urb to user read buffer */
		/* basicaly I need to take routines from ftdi-0.5 library */
		/* in mean time simply copy all to recv. buffer */
	        /*  */ 
	        wake = byte_fifo_put(&priv->read_fifo,data+2,urb->actual_length-2);
		dbg2("intr_callback: %d bytes pushed",wake);
		pushed_fifo_size = byte_fifo_get_actual_size(&priv->read_fifo);
		dbg2("size of read_fifo: %d ",pushed_fifo_size);
		/*  */
/* 		val_turns = ep->in_callback_process++ ;  */
/* 		val_bytes = ep->in_callback_bytes += urb->actual_length ;  */

	    spin_unlock_irqrestore( &priv->in_lock, flags );


	    if (debug) { 

	    if (!((priv->in_callback_turns) % 1000)) 
		for(i=0;i<urb->actual_length;i++) dbg3("turn: %lu data[%3d] %2X",priv->in_callback_turns,
						      i,(int)data[i]);
	    }


		if ( wake ) {
		  dbg2 ( " %s %s - waking-up read_wait",__FILE__,__FUNCTION__);
		  wake_up_interruptible(&priv->read_wait);	
		  }
	  }/* if urb_actual_length > 2 ) */

	}	

	urb->actual_length = 0 ; 
	
	urb->dev = priv->dev ; 

	if((ret = usb_submit_urb(urb)) <0){
		err("Error %d while submitting input URB", ret);
		wake_up_interruptible(&priv->read_wait);
	}
	

}





/* circular fifo functions : */

static inline size_t byte_fifo_space_left(struct byte_fifo *f) { 
  return (f->size_1 - (((size_t) (f->wp - f->rp)) & f->size_1)) ;
} 

static inline void update_rp (struct byte_fifo *f, size_t size) { 
  f->rp = f->buffer + (( f->rp - f->buffer + size) & f->size_1);  
} 

static inline size_t byte_fifo_get_actual_size(struct byte_fifo *f) { 
return (((size_t) (f->wp - f->rp)) & f->size_1) ;
}


  

/* Attention! : buffer_size has to be a power of 2 .. */

static size_t byte_fifo_create(struct byte_fifo *f, size_t buffer_size) { 


  unsigned char *data = (unsigned char*) kmalloc(buffer_size,GFP_KERNEL);


  f->buffer = NULL ; 

  if (!data) { 
    printk(KERN_ERR "ftdipio_buf: no memory for fifo\n");
    return 0;
  } 
  memset( data, 0, sizeof(data) );
  f->wp = data ; 
  f->rp = data ; /* same as wp : fifo empty */
  f->buffer = data ; 
  f->size = buffer_size ; 
  f->size_1 = buffer_size - 1 ; 

  return buffer_size ; 

}  

static void byte_fifo_destroy(struct byte_fifo *f) {
  if (f!=NULL) { 
 
    if (f->buffer!=NULL) kfree(f->buffer); 

  } 

} 


/* static int do_transfer_from_user(struct byte_fifo *f, */
/* 				 unsigned char *data,  */
/* 				 size_t size) { */

/*   size_t  upper_left = f->size - ((unsigned int) (f->wp - f->buffer));  */

/*   if (size > upper_left) { */
/*     copy_from_user(f->wp,data,upper_left);  */
/*     copy_from_user(f->buffer,data+upper_left,size-upper_left);  */
/*   } else {  */
/*     copy_from_user(f->wp,data,size); */
/*   }  */
/*   return size ;  */
/* }  */

/* static int do_transfer_to_user(struct byte_fifo *f, */
/* 				 unsigned char *data,  */
/* 				 size_t size) { */

/*   size_t  upper_left = f->size - ((unsigned int) (f->rp - f->buffer));  */

/*   if (size > upper_left) { */
/*     copy_to_user(data,f->rp,upper_left);  */
/*     copy_to_user(data+upper_left,f->buffer,size-upper_left);  */
/*   } else {  */
/*     copy_to_user(data,f->rp,size); */
/*   }  */
/*   return size ;  */
/* }  */

static int do_transfer_from_internal(struct byte_fifo *f,
				 unsigned char *data, 
				 size_t size) {

  size_t  upper_left = f->size - ((unsigned int) (f->wp - f->buffer)); 

  if (size > upper_left) {
    memcpy(f->wp,data,upper_left); 
    memcpy(f->buffer,data+upper_left,size-upper_left); 
  } else { 
    memcpy(f->wp,data,size);
  } 
  return size ; 
} 

static int do_transfer_to_internal(struct byte_fifo *f,
				 unsigned char *data, 
				 size_t size) {

  size_t  upper_left = f->size - ((unsigned int) (f->rp - f->buffer)); 

  if (size > upper_left) {
    memcpy(data,f->rp,upper_left); 
    memcpy(data+upper_left,f->buffer,size-upper_left); 
  } else { 
    memcpy(data,f->rp,size);
  } 
  return size ; 
} 



static inline void update_wp (struct byte_fifo *f, size_t size) { 
  f->wp = f->buffer + (( f->wp - f->buffer + size) & f->size_1);  
} 






static size_t byte_fifo_put(struct byte_fifo *f, unsigned char *data, size_t size) { 
  
  unsigned int fifo_space_left =  byte_fifo_space_left(f);
 
  if (!fifo_space_left) return 0 ;  
    do_transfer_from_internal(f,data,size) ;
    update_wp(f,size); 
    
  return size ; 

} 



static size_t byte_fifo_get( unsigned char *data, struct byte_fifo *f,size_t size) { 
  
unsigned int fifo_actual_size  = byte_fifo_get_actual_size(f) ; 
    
  if (!fifo_actual_size) return 0 ; 
  
  size = min(size,fifo_actual_size); 

    do_transfer_to_internal(f,data,size);
    update_rp(f,size);
  
    
  return size ; 


}


/* static size_t byte_fifo_put_from_internal(struct byte_fifo *f, unsigned char *data, size_t size) {  */
  
/*   unsigned int fifo_space_left =  byte_fifo_space_left(f);  */
/*   if (!fifo_space_left) return 0 ;  */
/*   if (size > fifo_space_left) {  */
/*     /\* fill fifo to the limits*\/ */
/*     do_transfer_from_internal(f,data,fifo_space_left); */
/*     update_wp(f,fifo_space_left); */
/*     size = fifo_space_left ;  */
/*   } else {  */
/*     do_transfer_from_user(f,data,size) ; */
/*     update_wp(f,size); */
/*   }  */
    
/*   return size ;  */


/* }  */



/* static size_t byte_fifo_get_to_internal(struct byte_fifo *f, unsigned char *data, size_t size) {  */
  
/* unsigned int fifo_actual_size  = byte_fifo_get_actual_size(f) ;  */
    
/*   if (!fifo_actual_size) return 0 ;  */
  
/*   size = min(size,fifo_actual_size);  */

/*     do_transfer_to_internal(f,data,size); */
/*     update_rp(f,size); */
  
    
/*   return size ;  */


/* } */

/* static size_t byte_fifo_get_to_user(struct byte_fifo *f, unsigned char *data, size_t size) {  */
  
/*   unsigned int fifo_actual_size = byte_fifo_get_actual_size(f) ; */
/*   if (!fifo_actual_size) return 0 ;  */
  
/*   size = min(size,fifo_actual_size);  */

/*     do_transfer_to_user(f,data,size); */
/*     update_rp(f,size); */
  
    
/*   return size ;  */


/* } */

/* static size_t byte_fifo_put_from_user(struct byte_fifo *f, unsigned char *data, size_t size) {  */
  
/*   unsigned int fifo_space_left = byte_fifo_space_left(f)  ;  */

/*   if (size > fifo_space_left) {  */
/*     /\* fill fifo to the limits*\/ */
/*     do_transfer_from_user(f,data,fifo_space_left); */
/*     update_wp(f,fifo_space_left); */
/*     size = fifo_space_left ;  */
/*   } else {  */
/*     do_transfer_from_user(f,data,size) ; */
/*     update_wp(f,size); */
/*   }  */
    
/*   return size ;  */


/* }  */











/* ----------------------------------------------------------------------- */

/* Hardware access functions */

/* static int */
/* mycontrolmsg(const char *funcname, */
/* 	     struct ftdipio_priv *priv, unsigned pipe_dir, */
/* 	     __u8 request, __u8 requesttype, __u16 value, */
/* 	     __u16 index, void *data, __u16 size) */
/* { */
/* 	int ret=0, tries = stall_tries; */

/* 	/\* Make sure the device was not disconnected *\/ */
/* 	if(down_interruptible(&priv->dev_sem)) */
/* 		return -ERESTARTSYS; */
/* 	if(!priv->dev) { */
/* 		up(&priv->dev_sem); */
/* 		return -ENOLINK; */
/* 	} */

/* 	/\* Dirty hack for retransmission: stalls and fails all the time */
/* 	   without this on the hardware we tested. *\/ */
/* 	while(tries--) { */
/* 		ret = usb_control_msg(priv->dev, */
/* 		    usb_sndctrlpipe(priv->dev,0) |pipe_dir, */
/* 		    request, requesttype, value, */
/* 		    index, data, size, */
/* 		    HZ); */
/* 		if(ret != -EPIPE) */
/* 			break; */
/* 		dbg2("Stalled, remaining %d tries", tries); */
/* 	} */
/* 	up(&priv->dev_sem); */
/* 	if(ret <0) { */
/* 		err("%s: usb_control_msg returns %d", */
/* 				funcname, ret); */
/* 		return -EIO; */
/* 	} */
/* 	return 0; */
/* } */



/* scratch : 






*/
