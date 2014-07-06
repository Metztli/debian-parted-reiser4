/*
    libparted - a library for manipulating disk partitions
    Copyright (C) 2000, 2009 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config.h>

#include <parted/parted.h>
#include <parted/endian.h>

#if ENABLE_NLS
#  include <libintl.h>
#  define _(String) dgettext (PACKAGE, String)
#else
#  define _(String) (String)
#endif /* ENABLE_NLS */

#include <unistd.h>

#define BTRFS_BLOCK_SIZES       ((int[2]){1024, 0})
#define BTRFS_SUPER_INFO_SIZE 4096
#define BTRFS_SUPER_INFO_OFFSET (64 * 1024)

//Should be definitive, as of v0.18
#define BTRFS_SIGNATURE	"_BHRfS_M"

static PedGeometry*
btrfs_probe (PedGeometry* geom)
{
	char	buf[BTRFS_SUPER_INFO_SIZE];

	if (!ped_geometry_read (geom, buf, (BTRFS_SUPER_INFO_OFFSET / 512), (BTRFS_SUPER_INFO_SIZE / 512)))
		return 0;

	if (strncmp (BTRFS_SIGNATURE, buf + 64, strlen (BTRFS_SIGNATURE)) == 0)
		return ped_geometry_new (geom->dev, geom->start, geom->length);
	else
		return NULL;
}

static PedFileSystemOps btrfs_ops = {
	probe:		btrfs_probe,
};

static PedFileSystemType btrfs_type = {
	next:	NULL,
	ops:	&btrfs_ops,
	name:	"btrfs",
	block_sizes: BTRFS_BLOCK_SIZES
};

void
ped_file_system_btrfs_init ()
{
	ped_file_system_type_register (&btrfs_type);
}

void
ped_file_system_btrfs_done ()
{
	ped_file_system_type_unregister (&btrfs_type);
}
