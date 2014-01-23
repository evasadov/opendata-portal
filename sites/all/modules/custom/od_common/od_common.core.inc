<?php
/**
 * @file
 */

/**
 * Copies entity_form to node. Creates new bundle if needed and creates not existing fields if needed.
 * @param $ef
 * @param $node_type
 * @param array $options
 *  keys are
 *    save_node (default to FALSE) - Saves node immediately in this function
 *    create_fields (default to TRUE) - Creates not existing fields for node's bundle
 * @return object
 *  Created node
 */
function common_copy_ef_to_node($ef, $node_type, $options = array()) {
  static $default_options = array(
    'save_node' => FALSE,
    'create_fields' => TRUE,
    //'create_node_type' => TRUE,
  );

  //Merge options
  $options = array_merge($default_options, $options);

  //Options' shortcuts
  $save_node = $options['save_node'];

  $node_types = node_type_get_types();
  if (!isset($node_types[$node_type])) {
    $fields = (object) array(
      'type' => (string) $node_type,
      'name' => 'Node type '. $node_type . ' created by rules action',
      'base' => 'node_content',
      'has_title' => FALSE,
      'title_label' => 'title label',
      'description' => 'This node type has been created by rules action',
      'help' => '',
      'custom' => TRUE,
      'modified' => FALSE,
      'locked' => FALSE,
      'disabled' => FALSE,
      'module' => 'common',
    );

    node_type_save($fields);
  }

  $wrapper = entity_metadata_wrapper('entityform', $ef);
  $pi = $wrapper->getPropertyInfo();

  $node = new stdClass();
  $node->type = $node_type;
  $node->title = 'Entity form submission copy from ' . date('c');
  $node->uid = $GLOBALS['user']->uid;
  node_object_prepare($node);
  $node_wrapper = entity_metadata_wrapper('node', $node);
  $node_pi = $node_wrapper->getPropertyInfo();
  foreach ($pi as $property => $info) {
    if (!empty($info['field'])) {
      $node_property = $property;
      $field_is_mapped = ((!empty($options['field_mappings'][$property])));
      if (isset($options['field_mappings']) && $field_is_mapped) {
        $node_property = $options['field_mappings'][$property];
      }

      //If we need to create the field in case if it doesn't exist
      $instance = NULL; //Set it to null just to be sure that we don't have an old value from previous cycle
      if (!($instance = field_info_instance('node', $node_property, $node_type)) && $options['create_fields']) {
        $instance = field_info_instance('entityform', $node_property, $ef->type);
        $instance['entity_type'] = 'node';
        $instance['bundle'] = $node_type;

        $instance = field_create_instance($instance);
      }

      //And finally if field's instance exists and we have filled property
      if ((!isset($options['field_mappings']) || $field_is_mapped) && $instance && isset($ef->{$property})) {
        $node->{$node_property} = $ef->{$property}; //we copy it
      }
    }
  }

  //And save node immediately if needed
  if ($save_node) {
    entity_save('node', $node);
  }

  return $node;
}