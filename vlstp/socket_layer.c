#include <string.h>

#include "socket_layer.h"

void fd_create(fdset_t* fdSet)
{
	if (fdSet) {
		fdSet->size = 0;
		fdSet->numready = 0;
		FD_ZERO(&fdSet->fdread);
		FD_ZERO(&fdSet->fdwrite);
		FD_ZERO(&fdSet->fdexcept);
	}
}
int fd_select(fdset_t* fdSet, unsigned long ms )
{
	struct timeval tv;
	if (fdSet) {
		tv.tv_sec = ( ms / 1000 );
		tv.tv_usec = ( ms % 1000 ) * 1000;
		fdSet->numready = select(fdSet->size, &fdSet->fdread,
		                         &fdSet->fdwrite, &fdSet->fdexcept, &tv);
		return fdSet->numready;
	}
	return 0;
}

int fd_ready_read(int fd, fdset_t* fdSet)
{
	if ( fdSet == 0 ) return -1;
	return FD_ISSET(fd, &fdSet->fdread);
}

int fd_ready_write(int fd, fdset_t* fdSet)
{
	if ( fdSet == 0 ) return -1;
	return FD_ISSET(fd, &fdSet->fdwrite);
}

void fd_set_read(int fd, fdset_t * fdSet )
{
	if ( fdSet )
	{
		FD_SET(fd, &fdSet->fdread);
		fdSet->size = ( fd + 1 > fdSet->size ? fd + 1 : fdSet->size );
	}
}
void fd_set_write(int fd, fdset_t * fdSet )
{
	if ( fdSet )
	{
		FD_SET(fd, &fdSet->fdwrite);
		fdSet->size = ( fd + 1 > fdSet->size ? fd + 1 : fdSet->size );
	}
}

int fd_set_noblock (int fd, int noblock)
{
	int opt;
	opt = fcntl(fd, F_GETFL, 0);
	if ( noblock == 1 )
		opt |= O_NONBLOCK;
	else
		opt &= (~O_NONBLOCK);
	return fcntl(fd, F_SETFL, opt );
}

int fd_bind_local ( int fd , char *addr , int port )
{
	int opt = 1;
	int len = sizeof ( opt );
	struct sockaddr_in addrin;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, len);
	addrin.sin_family = AF_INET;
	if ( addr != NULL && *addr != '\0' )
		addrin.sin_addr.s_addr = inet_addr(addr);
	else
		addrin.sin_addr.s_addr = htonl (INADDR_ANY);
	addrin.sin_port = htons(port);
	if ( bind(fd, (struct sockaddr*)&addrin, sizeof(struct sockaddr)) == -1 )
	{
		perror("bind local error:");
		return -1;
	}
	return 0;
}

int fd_bind_device ( int fd , char *devname )
{
	struct ifreq if_dev;
	if (devname != 0)
	{
		//	  memset(&if_dev,0,sizeof(struct ifreq));
		strncpy(if_dev.ifr_name, devname, IFNAMSIZ);
		if ( setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE,
		                (char*)&if_dev, sizeof(if_dev)) == -1 ) {
			perror("bind to device:");
			return -1;
		}
	}
	return 0;
}
int  fd_create_socket( int proto, char *devname , char * local_addr, int port )
{
	int fd;
	fd = socket(AF_INET, proto, 0);
	if ( fd == -1 )
	{
		perror("fd_create_socket:");
		return -1;
	}

	if ( fd_bind_local(fd, local_addr, port ) == -1 ) {
		close(fd);
		fd = -1;
		return -1;
	}
	if ( fd_bind_device (fd, devname ) == -1 ) {
		close(fd);
		fd = -1;
		return -1;
	}

	return fd;
}

int fd_write( int fd, unsigned char * data, int len )
{
	int bytes , write_bytes = len, writes = 0 ;
	unsigned char * output = data;
	do {
		bytes = send (fd , output , write_bytes, 0 );
		if ( bytes == -1 )
		{
			if (errno == EAGAIN )
			{
				return writes;
			}
			else
				return -1;
		}
		else if ( bytes == 0 )
			return -1;
		output += bytes;
		write_bytes -= bytes;
		writes += bytes;
	} while (write_bytes > 0 );
	return writes;
}

int fd_read( int fd , unsigned char* buf, int len )
{
	int bytes;
	bytes = recv(fd, buf, len, 0);
	if (bytes < 0 )
	{
		if (errno == EAGAIN )
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	if ( bytes == 0 )
	{
		return -1;
	}
	return bytes;
}

