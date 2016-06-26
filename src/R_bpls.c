int doList(const char *path) 
{
    ADIOS_FILE  *fp;
    int     grpid;
    int     status;
    int     mpi_comm_dummy=0;
    int     nGroupsMatched=0;
    int     nGroups; // number of groups
    char  **group_namelist;
    char    init_params[128];
    int     adios_verbose=2;

    if (verbose>1) printf("\nADIOS BP open: read header info from %s\n", path);

    // initialize BP reader
    if (verbose>1) adios_verbose = 3; // print info lines
    if (verbose>2) adios_verbose = 4; // print debug lines
    sprintf (init_params, "verbose=%d", adios_verbose);
    if (hidden_attrs)
        strcat (init_params, ";show_hidden_attrs");
    status = adios_read_init_method (ADIOS_READ_METHOD_BP, mpi_comm_dummy, init_params);
    if (status) {
        fprintf(stderr, "Error: %s\n", adios_errmsg());
        bpexit(6, 0);
    }

    // open the BP file
    fp = adios_read_open_file (path, ADIOS_READ_METHOD_BP, mpi_comm_dummy); 
    if (fp == NULL) {
        //fprintf(stderr, "Error: %s\n", adios_errmsg());
        bpexit(7, 0);
    }

    // get number of groups
    nGroups = adios_get_grouplist (fp, &group_namelist);

    //, variables, timesteps, and attributes 
    // all parameters are integers, 
    // besides the last parameter, which is an array of strings for holding the list of group names
    //ntsteps = fp->tidx_stop - fp->tidx_start + 1;
    if (verbose) {
        printf ("File info:\n");
        printf ("  of groups:     %d\n", nGroups);
        printf ("  of variables:  %d\n", fp->nvars);
        printf ("  of attributes: %d\n", fp->nattrs);
        printf ("  of meshes:     %d\n", fp->nmeshes);
        printf ("  time steps:    %d - %d\n", fp->current_step, fp->last_step);
        print_file_size(fp->file_size);
        printf ("  bp version:    %d\n", fp->version);
        printf ("  endianness:    %s\n", (fp->endianness ? "Big Endian" : "Little Endian"));
        if (longopt) 
            printf ("  statistics:    Min / Max / Avg / Std_dev\n");
        printf ("\n");
    }

    // Print out the meshes in the file
    if (listmeshes) {
        printMeshes(fp);
    }

    if (grpmask) {
        // each group has to be handled separately
        for (grpid=0; grpid < nGroups; grpid++) {
            if (!grpMatchesMask(group_namelist[grpid]))
                continue;
            nGroupsMatched++;
            if (!dump) fprintf(outf, "Group %s:\n", group_namelist[grpid]);
            status = adios_group_view (fp, grpid);
            if (status) {
                fprintf(stderr, "Error: %s\n", adios_errmsg());
                bpexit(8, fp);
            }

            doList_group (fp);

            adios_group_view (fp, -1); // reset full view (for next group view)
        }
    } else {
        doList_group (fp);
    }
    
    if (grpmask != NULL && nGroupsMatched == 0) {
        fprintf(stderr, "\nError: None of the groups matched the group mask you provided: %s\n", grpmask);
        return 4;
    }
    if (nmasks > 0 && nVarsMatched == 0) {
        fprintf(stderr, "\nError: None of the variables matched any name/regexp you provided\n");
        return 4;
    }
    adios_read_close (fp);
    return 0;
}
