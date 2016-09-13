

#ifndef _FTDIPIO_H
#define _FTDIPIO_H

struct byte_fifo { 
  unsigned char *wp ; 
  unsigned char *rp ; 
  unsigned char *buffer; 
  size_t size ;
  size_t size_1 ; 
  /* spinlock_t lock ; */ 
};

#define MAX_NR_FTDIPIO_DEVS    4
#define MAX_FTDIPIO_WRITE_BUF  4096
#define MAX_FTDIPIO_READ_BUF   4096
#define MAX_FTDIPIO_URB_BUF    64
#define MAX_IN_URB_TIMEOUT     8
#define MAX_IN_URB_PROBLEM     8
#define MAX_OUT_URB_TIMEOUT    8
#define NAK_TIMEOUT (HZ)


struct ftdipio_eeprom {
    int vendor_id;
    int product_id;
 
    int self_powered;
    int remote_wakeup;
    int BM_type_chip;
 
    int in_is_isochronous;
    int out_is_isochronous;
    int suspend_pull_downs;
 
    int use_serial;
    int change_usb_version;
    int usb_version;
    int max_power;
                                                                                
    char *manufacturer;
    char *product;
    char *serial;
};



/* private state */
struct ftdipio_priv {
	struct usb_device   *dev; /* USB device handle */
	struct usb_endpoint_descriptor *in_ep;
        struct usb_endpoint_descriptor *out_ep;
        
	struct urb *in_urb;
  unsigned char  *in_buf_urb; 
  unsigned int in_size_urb ; 
  __u8 in_endpointAddr ; 

        struct urb *out_urb; 
  unsigned char  *out_buf_urb;
  unsigned int out_size_urb ;
  __u8 out_endpointAddr ; 
  
 
  struct byte_fifo read_fifo ; 
  struct byte_fifo write_fifo ;

  unsigned char *read_buffer; 
  unsigned char *write_buffer; 

	devfs_handle_t devfs;

	int subminor; /* which minor dev # ? */


	spinlock_t in_lock; /* protects input process*/
        spinlock_t out_lock; /* protects input process*/
	wait_queue_head_t read_wait;
        wait_queue_head_t write_wait;

	int opened;
	struct semaphore open_sem; /* protects ->opened */
	struct semaphore dev_sem; /* protects ->dev */
        struct semaphore gen_sem; 

  atomic_t in_urb_timeout ; 
  atomic_t in_urb_problem ; 
  atomic_t out_urb_timeout ;
  atomic_t out_urb_problem ;
  atomic_t dev_connected ; 
  atomic_t in_to_read ; 
  
   unsigned long in_callback_turns ; 

   unsigned char latency ;
   int bigbang ; 

};



/* Use ’k’ as magic number */
#define FTDIPIO_IOC_MAGIC 0x41

#define FTDIPIO_IOC_RESET _IO(FTDIPIO_IOC_MAGIC, 0)

#define FTDIPIO_IOC_PURGE_BUFFERS _IO(FTDIPIO_IOC_MAGIC, 1)
#define FTDIPIO_IOC_SET_LATENCY_TIMER _IOW(FTDIPIO_IOC_MAGIC, 2, unsigned char)
#define FTDIPIO_IOC_GET_LATENCY_TIMER _IOR(FTDIPIO_IOC_MAGIC, 3, unsigned char)
#define FTDIPIO_IOC_ENABLE_BITBANG _IO(FTDIPIO_IOC_MAGIC, 4)
#define FTDIPIO_IOC_DISEABLE_BITBANG _IO(FTDIPIO_IOC_MAGIC, 5)
#define FTDIPIO_IOC_READ_PINS _IOR(FTDIPIO_IOC_MAGIC, 6, pins)
#define FTDIPIO_IOC_WRITE_EEPROM _IOW(FTDIPIO_IOC_MAGIC, 7, struct ftdipio_eeprom)
#define FTDIPIO_IOC_READ_EEPROM  _IOR(FTDIPIO_IOC_MAGIC, 8, struct ftdipio_eeprom)
#define FTDIPIO_IOC_READ_ID _IOR(FTDIPIO_IOC_MAGIC, 9, 256)
#define FTDIPIO_IOC_READ_SERIAL _IOR(FTDIPIO_IOC_MAGIC,10, 256)

#define FTDIPIO_IOCHARDRESET _IO(FTDIPIO_IOC_MAGIC, 15) /* debugging tool */
#define FTDIPIO_IOC_MAXNR 15

/* Prototypes */
static void *ftdipio_probe (struct usb_device *dev, unsigned ifnum,
			    const struct usb_device_id *id);
static void ftdipio_disconnect(struct usb_device *dev, void *ptr);
static int ftdipio_open(struct inode *inode, struct file *file);
static int ftdipio_release(struct inode *inode, struct file *file);
static ssize_t ftdipio_write(struct file *file, const char *buffer,
			     size_t count, loff_t *pos);
static ssize_t ftdipio_read(struct file *file, char *buffer,
			    size_t count, loff_t *unused_pos);
static int ftdipio_ioctl(struct inode *inode, struct file *file,
			 unsigned cmd, unsigned long arg);
static unsigned ftdipio_poll(struct file *file, poll_table *wait);
static loff_t ftdipio_llseek(struct file * file, loff_t offset, int orig);
static void intr_callback(struct urb *urb);
/* byte fifo functions */ 
static size_t byte_fifo_create(struct byte_fifo *f, size_t buffer_size);
static void byte_fifo_destroy(struct byte_fifo *f); 
static inline size_t byte_fifo_space_left(struct byte_fifo *f);
static inline void update_rp (struct byte_fifo *f, size_t size);
static inline void update_wp (struct byte_fifo *f, size_t size);
static inline size_t byte_fifo_get_actual_size(struct byte_fifo *f);

static size_t byte_fifo_put(struct byte_fifo *f, unsigned char *data, size_t size); 
static size_t byte_fifo_get(unsigned char *data, struct byte_fifo *f, size_t size); 



/* Some print macros */
#ifdef dbg
#undef dbg
#endif
#ifdef info
#undef info
#endif
#ifdef err
#undef err
#endif
#define info(args...) \
    ({ printk(KERN_INFO "FTDI245: " args); \
       printk("\n"); })
#define err(args...) \
    ({ printk(KERN_ERR "FTDI245: " args); \
       printk("\n"); })
#define dbgprint(fmt, args...) \
    ({ printk(KERN_DEBUG "FTDI245: %s: " fmt, __FUNCTION__ , ##args); \
       printk("\n"); })
#define dbg(args...) \
    ({ if(debug >= 1) dbgprint(args); })
#define dbg2(args...) \
    ({ if(debug >= 2) dbgprint(args); })
#define dbg3(args...) \
    ({ if(debug >= 3) dbgprint(args); })

/* ----------------------------------------------------------------------- */




#endif 
