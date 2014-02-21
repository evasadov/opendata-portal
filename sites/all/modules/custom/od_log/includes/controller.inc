<?php

/**
 * Create opendata log.
 */
function od_log($message, $importance = 'status', $type = NULL, $organization_id = NULL, $dataset_id = NULL) {
  $vars = array();
  $vars['message'] = $message;
  $vars['field_log_importance_type'] = array(LANGUAGE_NONE => array('0' => array('value' => $importance)));
  $vars['field_log_event_type'] = !empty($type) ? array(LANGUAGE_NONE => array('0' => array('value' => $type))) : array();

  if (!empty($organization_id)) {
    $vars['field_organization'] = array(LANGUAGE_NONE => array('0' => array('target_id' => $organization_id)));
  }

  if (!empty($dataset_id)) {
    $vars['field_dataset_ref'] = array(LANGUAGE_NONE => array('0' => array('target_id' => $dataset_id)));
  }

  drupal_set_message($message, $importance);
  log_event($vars);
}