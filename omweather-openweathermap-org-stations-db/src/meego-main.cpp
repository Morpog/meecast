/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of omweather-openweathermap-org-source - MeeCast
 *
 * Copyright (C) 2013 Vlad Vasilyeu
 * 	for the code
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "meego-main.h"

/*******************************************************************************/

int
convert_station_openweathermaporg_data(const char *days_data_path, const char *result_file, const char *hours_data_path, const char *current_data_path ){
 
    xmlDoc  *doc = NULL;
    xmlNode *root_node = NULL;
    int     days_number = -1;
    char    buffer[1024],
            *delimiter = NULL;
    
    if(!days_data_path)
        return -1;

}

/*******************************************************************************/
int
main(int argc, char *argv[]){
    int result; 
    if (argc < 4) {
        fprintf(stderr, "openweathermaporg <input_days_file> <output_file> <input_hours_file> <input_current_file>\n");
        return -1;
    }
    result = convert_station_openweathermaporg_data(argv[1], argv[2], argv[3], argv[4]);
    //fprintf(stderr, "\nresult = %d\n", result);
    return result;
}
