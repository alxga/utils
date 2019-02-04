/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#ifndef MPIMSG_HEADER_FILE_INCLUDED
#define MPIMSG_HEADER_FILE_INCLUDED

#define WMSG_READY (1<<0)
#define WMSG_RESULTS (1<<1)
#define MSG_RUN (1<<2)
#define MSG_QUIT (1<<3)

#define MSG(_cmd, _data) ((_cmd) << 24 | _data)
#define MSG_CMD(_msg) ((_msg) >> 24)
#define MSG_DATA(_msg) ((_msg) & 0x00FFFFFF)

#endif // MPIMSG_HEADER_FILE_INCLUDED
