/* $Id: io.c 738 2008-03-12 16:14:41Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : io.c
 * 
 * Assumtions we make in this code:
 * a) input file defined by INPUT_FILE is valid XML for Circles Model
 * b) Only one type of agent in input (Circles)
 * c) id, x, y, fx, fy, radius defined for all Circle agents
 */

#include "circles.h"
#include <string.h>

int parseXML(const char *filename,
            XML_StartElementHandler startElemCallBackFunc,
            XML_EndElementHandler endElemCallBackFunc,
            XML_CharacterDataHandler charDataCallbackFunc,
            void *userData) {
    
    FILE *fp;
    int done, status = OK;
    size_t outLen;
    char buf[BUFSIZE];
    XML_Parser parser;
    
    /* open file */
    fp = fopen(filename, "r");
    if (fp == NULL) return FAIL;
        
    /* create XML parser */
    parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, userData);
    XML_SetCharacterDataHandler(parser, charDataCallbackFunc);
    XML_SetElementHandler(parser, startElemCallBackFunc, endElemCallBackFunc);   
    
    /* parse input data */
    do {    
        outLen = fread(buf, 1, sizeof(buf), fp);
        done = (outLen < sizeof(buf));
        
        if (!XML_Parse(parser, buf, (int)outLen, done))
        {
            /* on error */
            status = FAIL;
            break;
        }
        
    } while (!done);
    
    /* clean up */
    fclose(fp);
    XML_ParserFree(parser);
    
    return status;
}

int writeOutputXML(void) {
    
    FILE *fp;
    char filename[256];
    
    /* open file for writing */
    snprintf(filename, 256, "%d_of_%d%s", env_rank + 1, env_size, OUTFILE_SUFFIX);
    fp = fopen(filename, "w");
    if (fp == NULL) return FAIL;
    
    /* write header */
    fprintf(fp, "<states>\n");
    fprintf(fp, "\t<itno>%d</itno>\n", iter - 1);
    fprintf(fp, "\t<environment></environment>\n");
    
    /* write data of each agent */
    LOOP_THRU_AGENTS 
    {
        fprintf(fp, "\t<xagent>\n");
        fprintf(fp, "\t\t<name>Circle</name>\n");
        fprintf(fp, "\t\t<id>%d</id>\n", agent_get_id());
        fprintf(fp, "\t\t<x>%f</x>\n", agent_get_x());
        fprintf(fp, "\t\t<y>%f</y>\n", agent_get_y());
        fprintf(fp, "\t\t<fx>%f</fx>\n", agent_get_fx());
        fprintf(fp, "\t\t<fy>%f</fy>\n", agent_get_fy());
        fprintf(fp, "\t\t<radius>%f</radius>\n", agent_get_radius());
        fprintf(fp, "\t</xagent>\n");
    }
    
    /* write footer and close file */
    fprintf(fp, "</states>\n");
    fclose(fp);
    
    printf("* Output file written to %s\n", filename);
    return OK;
}
