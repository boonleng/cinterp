/*
 * Name of source file:
 * wtfilter.c
 *
 * Author:
 * Boon Leng Cheong
 *
 * Purpose:
 * Incoporate IRISRAW framework for reading & writing IRIS raw data
 * file; and WF framework for wind turbine filtering
 *
 * Results:
 * Upon a successful executing. Data processing using WF framework 
 * will be resulted and an IRIS raw data file will be generated
 * retaining all the original meta data in the headers.
 *
 * Revision History:
 * 2014/06/17 - Version 1.0.3
 *            - Improved failure handling
 *            - Added timing measurements
 * 2014/05/27 - Version 1.0.2
 *            - Added logic to skip processing DBT if DBZ is present.
 *            - Added default method in help text output.
 * 2014/05/24 - Version 1.0.1
 *            - Using updated irisraw.h
 * 2014/05/01 - Version 1.0 created.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "irisraw.h"
#include "wf.h"

#define WTFILTER_VERSION    "1.0.3"

/*
 *
 *   M  A  I  N
 *
 */
int main(int argc,                    /* argument count  */
         char **argv                  /* argument values */
)
{
    int k;
    int s;
    int p;
    int sweepIndex;
    int productIndex;
    char c;
    char v = 0;
    char simCase = '3';
    char interpDBZOnly = 0;
    char configFile[WF_STRLEN] = "";
    char outputFile[WF_STRLEN] = "cleanup.iri";
    char userSweepIds[WF_MAX_SWEEP_COUNT];       /* Sweep Ids according to IRIS convention, origin at 1 */
    int numberOfUserSweepIds = 0;                /* Number of sweeps to process                         */
    size_t globalId;
    size_t i;

    WFConfigStruct *config;
    IRVolumeStruct *volume;
    
    struct timeval t1, t2, t3, t4;
    
    /* Stop #1 */
    gettimeofday(&t1, NULL);
    
    /* Parse out the user input */
    while ((c = getopt(argc, argv, "itbB123vc:s:o:h")) != -1) {
        switch (c) {
            case 'i': /* Undocumented. For developers only. */
            case 't': /* Undocumented. For developers only. */
            case 'b': /* Undocumented. For developers only. */
            case 'B': /* Undocumented. For developers only. */
            case '1':
            case '2':
            case '3':
                simCase = c;
                break;
            case 'v':
                v++;
                break;
            case 'c':
                strncpy(configFile, optarg, WF_STRLEN);
                break;
            case 's':
                userSweepIds[numberOfUserSweepIds] = (char)atoi(optarg);
                printf("%s : Input sweep[%d] = %d\n", now(), numberOfUserSweepIds, userSweepIds[numberOfUserSweepIds]);
                numberOfUserSweepIds++;
                break;
            case 'o':
                strncpy(outputFile, optarg, WF_STRLEN);
                break;
            case 'h':
            default:
                printf("%s - Wind Turbine Filter v%s\n\n"
                       "%s [-v] [-c CONFIG_FILE] [-o OUTPUT_FILE] -s SWEEP_NUMBER INPUT_FILE\n\n"
                       "processes the radar data file \033[4mFILE\033[24m by using the wind turbine locations\n"
                       "specified in the plain text file \033[4mCONFIG_FILE\033[24m.\n\n"
                       "OPTIONS:\n"
                       "    -1               interpolation method 1, bilinear\n"
                       "    -2               interpolation method 2, selective bilinear\n"
                       "    -3               interpolation method 3, inverse-distance (default)\n"
                       "    -c CONFIG_FILE   uses CONFIG_FILE as the configuration setup\n"
                       "    -s SWEEP_NUMBER  adds a sweep (origin at 1) to process\n"
                       "    -o OUTPUT_FILE   specifies the output file\n"
                       "    -v               increases verbosity\n\n"
                       "EXAMPLES:\n"
                       "    The simplest example:\n\n"
                       "        %s -s 24 WSO_201402180930_CONVOL.iri\n\n"
                       "    processes input file \033[4mWSO_201402180930_CONVOL.iri\033[24m on sweep \033[4m24\033[24m. All other\n"
                       "    sweeps within the volume will be left intact and duplicated to the output\n"
                       "    file. Since it is not specified, the default configuration file \033[4mwtloc.txt\033[24m\n"
                       "    will be used and the default output file \033[4mcleanup.iri\033[24m will be generated.\n\n"
                       "        %s -s 23 -s 24 -o output.iri WSO_201402180930_CONVOL.iri\n\n"
                       "    processes the input file \033[4mWSO_201402180930_CONVOL.iri\033[24m on sweeps \033[4m23\033[24m and \033[4m24\033[24m\n"
                       "    and generates output file \033[4moutput.iri\033[24m\n\n"
                       "        %s -2 -s 23 -s 24 -o output.iri WSO_201402180930_CONVOL.iri\n\n"
                       "    uses method \033[4m2\033[24m and processes the input file \033[4mWSO_201402180930_CONVOL.iri\033[24m\n"
                       "    on sweeps \033[4m23\033[24m and \033[4m24\033[24m and generates output file \033[4moutput.iri\033[24m\n\n"
                       "        %s -s 1 -o output.iri WSO_201402180930_DOPVOL1_A.iri\n\n"
                       "    processes the input file \033[4mWSO_201402180930_DOPVOL1_A.iri\033[24m and generates\n"
                       "    output file \033[4moutput.iri\033[24m on sweep \033[4m1\033[24m.\n\n"
                       "        %s -c wt.conf -s 1 -o out.iri WSO_201402180930_DOPVOL1_A.iri\n\n"
                       "    processes the input file \033[4mWSO_201402180930_DOPVOL1_A.iri\033[24m using the\n"
                       "    configuration file \033[4wt.conf\033[24m and generates output file \033[4mout.iri\033[24m. The\n"
                       "    processing will be performed on sweep \033[4m1\033[24m.\n\n",
                       argv[0], WTFILTER_VERSION, argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
                return EXIT_FAILURE;
                break;
        }
    } /* while */

    /* Get user configuration parameters */
    if (strlen(configFile) > 0 || v > 0) {
        config = WFConfigInitFromFile(configFile, v);
    } else {
        config = WFConfigInit();
    }
    
    /* Warn if no sweeps were specified */
    if (numberOfUserSweepIds == 0) {
        fprintf(stderr, "No sweeps were specified.\n");
        return EXIT_FAILURE;
    }

    /* Do a demo if no file is specified */
    if (argc < 2 || argv[optind] == NULL) {
        printf("No input file. Doing a demo.\n");
        volume = IRReadRawData("WSO_201402180930_CONVOL.iri", v);
        userSweepIds[numberOfUserSweepIds++] = 24;
    } else {
        printf("%s : Input file %s\n", now(), argv[optind]);
        volume = IRReadRawData(argv[optind], v);
    }
    if (volume == NULL) {
        printf("%s : Failed to read %s\n", now(), argv[optind]);
        return EXIT_FAILURE;
    }

    /* Stop #2 */
    gettimeofday(&t2, NULL);
    
    /* Construct a sweep unit */
    WFSweepStruct sweep;
    sweep.azimuthDelta = 360.0f / volume->numberOfRaysPerSweep;
    sweep.rangeDelta = volume->rangeDelta;
    sweep.rangeStart = volume->rangeStart;
    sweep.rangeCount = IR_MAX_RANGE_BINS;
    sweep.nyquistVelocity = volume->nyquistVelocity;
    sweep.rayCount = volume->numberOfRaysPerSweep;
    sweep.rays = (WFRayStruct *)malloc(sweep.rayCount * sizeof(WFRayStruct));
    sweep.rays[0].data = NULL;
 
    WFCountStruct counts;
    WFInterpFlag flag = 0;
    WFCellState *state = NULL;
    WFRadarCellId *cellIds = NULL;
    WFCoordStruct origin = WFCoordStructMake(volume->latitude, volume->longitude);
    
    /* Process all the user specified sweeps */
    for (k=0; k<numberOfUserSweepIds; k++) {
        
        /* Skip the sweep indices that are not present */
        sweepIndex = userSweepIds[k];
        if (sweepIndex > volume->numberOfSweeps) {
            continue;
        }
        
        /* IRIS reference starts at 1 but C reference index starts at 0 */
        s = sweepIndex - 1;
        
        /* Set up and gather sweep parameters */
        sweep.elevation = volume->sweepElevations[k];
        globalId = s * volume->numberOfRaysPerSweep * volume->numberOfProducts;
        for (i=0; i<sweep.rayCount; i++) {
            sweep.rays[i].azimuthBegin   = IR_DEGREE(volume->rays[globalId].startingAzimuth);
            sweep.rays[i].azimuthEnd     = IR_DEGREE(volume->rays[globalId].endingAzimuth);
            sweep.rays[i].elevationBegin = IR_DEGREE(volume->rays[globalId].startingElevation);
            sweep.rays[i].elevationEnd   = IR_DEGREE(volume->rays[globalId].endingElevation);
            globalId += volume->numberOfProducts;
            if (v) {
                printf("%s : E%.1f-%.1f  A%7.2f-%7.2f\n",
                       now(),
                       sweep.rays[i].elevationBegin,
                       sweep.rays[i].elevationEnd,
                       sweep.rays[i].azimuthBegin,
                       sweep.rays[i].azimuthEnd);
            }
        }

        /* Cell index of the turbine locations */
        WFCreateCensorRadarCellIds(&counts, &cellIds, &sweep, &origin, config);

        /* Check if volume has both DBT & DBZ. Assume there won't be two DBZ. */
        p = 0;
        for (i=0; i<volume->numberOfProducts; i++) {
            if (volume->products[i] == WF_MOMENT_S || volume->products[i] == WF_MOMENT_Z) {
                p++;
            }
        }
        if (p > 1) {
            interpDBZOnly = 1;
        }
        
        /* Go through product 1 (DBT), 2 (DBZ) and 3 (VEL), find the C reference index */
        for (productIndex=1; productIndex<=3; productIndex++) {
            p = IR_MAX_PRODS;
            for (i=0; i<volume->numberOfProducts; i++) {
                if (volume->products[i] == productIndex) {
                    p = (int)i;
                }
            }

            if (p == IR_MAX_PRODS) {
                printf("%s : Sweep %d product %d (%s) not found in the volume.\n",
                       now(),
                       sweepIndex,
                       productIndex,
                       IR_PROD_STR(productIndex));
                continue;
            }
            
            if (productIndex == 1 && interpDBZOnly) {
                printf("%s : Sweep %d has DBZ, skip DBT.\n",
                       now(),
                       sweepIndex);
                continue;
            }
            
            printf("%s : Sweep %d product %d (%s) @ [s=%d p=%d]\n",
                   now(),
                   sweepIndex,
                   productIndex,
                   IR_PROD_STR(productIndex),
                   k,
                   p);
            
            /* Gather pointers of product rays. Start the globalId at the product of interest */
            globalId = s * volume->numberOfRaysPerSweep * volume->numberOfProducts + p;
            
            /* Grab the data pointers from sweepIndex, productIndex */
            sweep.moment = volume->products[p];
            sweep.rangeCount = volume->rays[globalId].numberOfRangeBins;
            
            for (i=0; i<sweep.rayCount; i++) {
                sweep.rays[i].data = &volume->rays[globalId].value[0];
                globalId += volume->numberOfProducts;
            }

            if (productIndex == WF_MOMENT_S) {
                flag = WF_INTERP_FLAG_GENERATE_CELL_STATE | WF_INTERP_FLAG_MAP_DB_TO_LINEAR;
            } else if (productIndex == WF_MOMENT_Z) {
                if (interpDBZOnly) {
                    flag = WF_INTERP_FLAG_GENERATE_CELL_STATE | WF_INTERP_FLAG_MAP_DB_TO_LINEAR;
                } else {
                    flag = WF_INTERP_FLAG_USE_CELL_STATE | WF_INTERP_FLAG_MAP_DB_TO_LINEAR;
                }
            } else if (productIndex == WF_MOMENT_V) {
                flag = WF_INTERP_FLAG_USE_CELL_STATE | WF_INTERP_FLAG_MAP_TO_COMPLEX;
            }

            if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE && state != NULL) {
                if (v > 2) {
                    printf("%s : wtfilter : Freeing existing state mask...\n", now());
                }
                free(state);
            }

            switch (simCase) {
                case 'i':
                    /* Simulating wind-turbine contamination, cluster id */
                    if (productIndex == WF_MOMENT_V) {
                        for (i=0; i<counts.turbinesMultipathAndPadding; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 6.0f + 5.0f * cellIds[i].clusterId;
                        }
                    } else {
                        for (i=0; i<counts.turbinesMultipathAndPadding; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 36.0f + 5.0f * cellIds[i].clusterId;
                        }
                    }
                    break;
                case 'b':
                case 'B':
                    /* Show cluster boundary */
                    if (productIndex == WF_MOMENT_V) {
                        for (i=(int)counts.turbinesMultipathAndPadding; i<counts.turbinesMultipathPaddingAndBoundary; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = -20.0f;
                        }
                    } else {
                        for (i=(int)counts.turbinesMultipathAndPadding; i<counts.turbinesMultipathPaddingAndBoundary; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 65.0f;
                        }
                    }
                    if (simCase == 'B') {
                        break;
                    }
                case 't':
                    /* Simulating wind-turbine contamination, location extension level */
                    if (productIndex == WF_MOMENT_V) {
                        for (i=0; i<counts.turbines; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 10.0f;
                        }
                        for (; i<counts.turbinesAndMultipath; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 20.0f;
                        }
                        for (; i<counts.turbinesMultipathAndPadding; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 30.0f;
                        }
                    } else {
                        for (i=0; i<counts.turbines; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 50.0f;
                        }
                        for (; i<counts.turbinesAndMultipath; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 40.0f;
                        }
                        for (; i<counts.turbinesMultipathAndPadding; i++) {
                            sweep.rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 35.0f;
                        }
                    }
                    if (!strcmp(outputFile, "cleanup.iri")) {
                        sprintf(outputFile, "turbine.iri");
                    }
                    break;
                case '1':
                    WFBilinearInterpData(&sweep, &state, cellIds, flag, &counts);
                    break;
                case '2':
                    WFSelectiveBilinearInterpData(&sweep, &state, cellIds, flag, &counts);
                    break;
                case '3':
                    WFInverseDistanceInterpData(&sweep, &state, cellIds, flag, &counts);
                    break;
                default:
                    break;
            } /* switch (simCase) */
        } /* for (p=1; p<=3; p++) */

        /* Release the cell indices after the sweep is done */
        WFReleaseCensorRadarCellIds(cellIds);
        
        free(state);
        state = NULL;

    } /* for (s=0; s<numberOfSweepIds; s++ ) */

    /* Stop #3 */
    gettimeofday(&t3, NULL);
    
    /* Generate output volume */
    IRWriteRawData(outputFile, volume, v);

    /* Be a good citizen & clean up after ourselves */
    free(sweep.rays);
    IRVolumeRelease(volume);

    /* Stop #4 */
    gettimeofday(&t4, NULL);
    
    /* Elapsed time summary */
    printf("%s : Elapsed time - PARTS = [  READ  PROC  WRITE ] secs\n"
           "  :  :   :              - DELTA = [ %5.2f %5.2f %5.2f  ]\n"
           "  :  :   :              - ACCUM = [ %5.2f %5.2f %5.2f  ]\n",
           now(),
           DTIME(t1, t2), DTIME(t2, t3), DTIME(t3, t4),
           DTIME(t1, t2), DTIME(t1, t3), DTIME(t1, t4));

    return EXIT_SUCCESS;
}

