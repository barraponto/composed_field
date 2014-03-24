<?php

/**
 * An extension to the default Views field handler to support composed fields.
 */
class composed_field_handler_field_composed_field extends views_handler_field_field {

  /**
   * Defines options for composed fields.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['composed_field'] = array(
      'contains' => array(
        'components' => array(
          'default' => array(),
        ),
      ),
    );
    return $options;
  }

  /**
   * Defines checkboxes for composed field components in a fieldset.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    /* Composed Fields are defined per-instance. Since Views uses the same
     * field handler for any entity and bundle (which allows for displaying
     * fields in different entities/bundles), we cannot load titles for
     * composed field components. They are not required anyway, odds are they'd
     * be empty anyway.
     */

    $field_name = $this->field_info['field_name'];
    $cardinalities = array();

    foreach ($this->field_info['bundles'] as $entity_type => $bundles) {
      foreach ($bundles as $bundle) {
        $instance = field_info_instance($entity_type, $field_name, $bundle);
        $cardinalities[] = $instance['widget']['settings']['number_of_subfields'];
      }
    }

    // Let's just get the maximum number of subfields.
    $cardinality = max($cardinalities);

    $form['composed_field'] = array(
      '#type' => 'fieldset',
      '#title' => t('Composed Field Components'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'components' => array(
        '#title' => t('Select the field components to display'),
        '#description' => t('If no component is selected, all will be displayed.'),
        '#type' => 'checkboxes',
        '#options' => $this->composed_field_component_options($cardinality),
        '#default_value' => $this->options['composed_field']['components'],
      ),
    );
  }

  /**
   * Generates options for the composed field components checkboxes.
   */
  private function composed_field_component_options($cardinality) {
    $options = array();
    foreach (range(1, $cardinality) as $subdelta) {
      // Let's call the components by it's "subdelta" position.
      $options[$subdelta] = t('Component #@subdelta', array('@subdelta' => $subdelta));
    }
    return $options;
  }

  /**
   * Filter out disabled components.
   */
  function set_items($values, $row_id) {
    $items = parent::set_items($values, $row_id);

    $components = array_filter($this->options['composed_field']['components']);
    // If no component was chosen, show all. This makes for a good default.
    if (!empty($components)) {
      foreach ($items as $delta => $item) {
        foreach ($item['rendered']['#item'] as $component => $value) {
          if (!in_array($component, $components)) {
            unset($items[$delta]['rendered']['#item'][$component]);
          }
        }
      }
    }

    return $items;
  }
}
