/*
 * Copyright (c) 2013      University of Wisconsin-La Crosse.
 *                         All rights reserved.
 * Copyright (c) 2013 Cisco Systems, Inc.  All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 * See COPYING in top-level directory.
 *
 * $HEADER$
 *
 *
 * Data Collection API
 * -------------------
 * This interface extends the "north bound" (user facing) interface with
 * functionlaity to support backed (or "south bound") readers.
 *
 * Readers should use this API to store netloc structures. The intention
 * of this interface is to abstract away the data storage mechanism from
 * the readers.
 */

#ifndef _NETLOC_DC_H_
#define _NETLOC_DC_H_

#include <netloc/autogen/config.h>
#include <netloc/rename.h>

#define _GNU_SOURCE // for asprintf
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <jansson.h>

#include <netloc.h>

/**********************************************************************
 * Enumerated types
 **********************************************************************/

/**********************************************************************
 *        Structures
 **********************************************************************/
/**
 * Data Connection Handle
 */
struct netloc_data_collection_handle_t {
    /** Point to the network */
    netloc_network_t *network;

    /** Status of the handle */
    bool is_open;
    bool is_read_only;

    /** Unique ID String */
    char * unique_id_str;

    /** Data URI */
    char * data_uri;

    /** Filename: Nodes */
    char * filename_nodes;

    /** Filename: Physical Paths */
    char * filename_physical_paths;

    /** Filename: Logical Paths */
    char * filename_logical_paths;

    /** Lookup table for all node information */
    netloc_dt_lookup_table_t *node_list;

    /** Lookup table for all edge information */
    netloc_dt_lookup_table_t *edges;

    /** JSON Object for nodes */
    json_t *node_data;
    json_t *node_data_acc;

    /** JSON Object for paths */
    json_t *path_data;
    json_t *path_data_acc;

    /** JSON Object for paths */
    json_t *phy_path_data;
    json_t *phy_path_data_acc;
};
typedef struct netloc_data_collection_handle_t netloc_data_collection_handle_t;

/**********************************************************************
 *        Datatype Support Functions
 **********************************************************************/
/**
 * Constructor for netloc_data_collection_handle_t
 *
 * User is responsible for calling the destructor on the handle.
 */
NETLOC_DECLSPEC netloc_data_collection_handle_t * netloc_dt_data_collection_handle_t_construct();

/**
 * Destructor for netloc_data_collection_handle_t
 *
 * \param handle A pointer to a netloc_data_collection_handle_t previously constructed.
 */
NETLOC_DECLSPEC int netloc_dt_data_collection_handle_t_destruct(netloc_data_collection_handle_t *handle);


/**********************************************************************
 * Data Collection API Functions
 **********************************************************************/
/**
 * Create a new data collection for this network.
 *
 * The user is responsible for calling the netloc_dt_data_collection_handle_t_destruct() function
 * on the pointer returned once finished with the handle.
 *
 * This function duplicates the network_t pointer passed to it, so the user is free to call the
 * the netloc_dt_network_t_destruct() function on the pointer when finished with it.
 *
 * netowrk
 * \param network Network information (must be complete, from a prior call to netloc_find_network)
 * \param dir Directory to store the .ndat files (Allowd to be NULL if current working directory)
 *
 * Returns
 *   NULL on error, and a valid data collection handle on success
 */
NETLOC_DECLSPEC netloc_data_collection_handle_t * netloc_dc_create(netloc_network_t *network, char * dir);

/**
 * Close a data collection handle
 * This may write out data if the handle was created in netloc_dc_create.
 *
 * The user is responsible for calling netloc_dt_data_collection_handle_t_destruct() on
 * the handle when finished with it. The close function does not destruct the handle.
 *
 * \param handle A valid pointer to a data collection handle
 *
 * Returns
 *   NETLOC_SUCCESS upon success
 *   NETLOC_ERROR otherwise
 */
NETLOC_DECLSPEC int netloc_dc_close(netloc_data_collection_handle_t *handle);

/**
 * Get the network information from the handle.
 *
 * \param handle A valid pointer to a data collection handle
 *
 * Returns
 *   NULL if no network information found
 *   Pointer to a netloc_network_t (caller is responsibe for deallocating this object)
 */
NETLOC_DECLSPEC netloc_network_t * netloc_dc_handle_get_network(netloc_data_collection_handle_t *handle);

/**
 * Get the unique_id_str for the specified handle
 *
 * \param handle A valid pointer to a data collection handle
 *
 * Returns
 *   NULL if handle is invalid, or has not unique_id_str
 *   Unique ID string for this handle (caller is responsible for deallocating the string)
 */
NETLOC_DECLSPEC char * netloc_dc_handle_get_unique_id_str(netloc_data_collection_handle_t *handle);

/**
 * Get the unique_id_str for the specified filename (so we might open it)
 *
 * \param filename Filename with network information
 *
 * Returns
 *   NULL if handle is invalid, or has not unique_id_str
 *   Unique ID string for this handle (caller is responsible for deallocating the string)
 */
NETLOC_DECLSPEC char * netloc_dc_handle_get_unique_id_str_filename(char *filename);

/**
 * Append netloc_node_t information to the data collection
 *
 * \param handle A valid pointer to a data collection handle
 * \param node   A pointer to the netloc_node_t to append
 *
 * Returns
 *   NETLOC_SUCCESS upon success
 *   NETLOC_ERROR otherwise
 */
NETLOC_DECLSPEC int netloc_dc_append_node(netloc_data_collection_handle_t *handle, netloc_node_t *node);

/**
 * Append netloc_edge_t information to the netloc_node_t structure
 *
 * This function makes a copy of the edge information before storing it on the node. So 
 * the user may reuse the edge, and is responsible for calling the edge destructor when
 * finished with it.
 *
 * \param handle A valid pointer to a data collection handle
 * \param node   A valid pointer to a netloc_node_t to append the edge to
 * \param edge   A valid pointer to the edge information to attach
 *
 * Returns
 *   NETLOC_SUCCESS upon success
 *   NETLOC_ERROR otherwise
 */
NETLOC_DECLSPEC int netloc_dc_append_edge_to_node(netloc_data_collection_handle_t *handle, netloc_node_t *node, netloc_edge_t *edge);

/**
 * Access a stored node by the physcial ID
 *
 * The user should -not- call the destructor on the returned value.
 *
 * \param handle A valid pointer to a data collection handle
 * \param phy_id The physical_id to search for
 *
 * Returns
 *   A pointer to the netloc_node_t with the specified physical_id
 *   or NULL if the physical_id is not found.
 */
NETLOC_DECLSPEC netloc_node_t * netloc_dc_get_node_by_physical_id(netloc_data_collection_handle_t *handle, char * phy_id);

/**
 * Append a path between two netloc_node_t objects
 * Each edge in this list will be appened to the data collection, if it is not already there.
 *
 * \param handle A valid pointer to a data collection handle
 * \param src_node_id Physical node id of the source
 * \param dest_node_id Physical node id of the destination
 * \param num_edges Number of edges in the edges array
 * \param edges Ordered array of edges from the source to the destination
 * \param is_logical If the path is a logical or physical path
 *
 * Returns
 *   NETLOC_SUCCESS upon success
 *   NETLOC_ERROR otherwise
 */
NETLOC_DECLSPEC int netloc_dc_append_path(netloc_data_collection_handle_t *handle,
                                          const char * src_node_id,
                                          const char * dest_node_id,
                                          int num_edges, netloc_edge_t **edges,
                                          bool is_logical);

/**
 * JJH TODO
 */
NETLOC_DECLSPEC int netloc_dc_compute_path_between_nodes(netloc_data_collection_handle_t *handle,
                                                         netloc_node_t *src_node,
                                                         netloc_node_t *dest_node,
                                                         int *num_edges,
                                                         netloc_edge_t ***edges,
                                                         bool is_logical);


/**
 * Pretty print the data collection to stdout (Debugging Support)
 *
 * \param handle A valid pointer to a data collection handle
 *
 */
NETLOC_DECLSPEC void netloc_dc_pretty_print(netloc_data_collection_handle_t *handle);

#endif // _NETLOC_DC_H_
