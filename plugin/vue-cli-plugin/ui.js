// This file will be loaded when the project is opened
module.exports = api => {

  console.log('has(eslint)', api.hasPlugin('eslint'))
  console.log('has(typescript)', api.hasPlugin('typescript'))

  //api.addClientAddon({
  //  id: 'vue-issue-demo1',
  //  url: 'http://localhost:8043/index.js'
  //})
  //
  //api.addView({
  //  id: 'vue-issue-demo1-view',
  //  name: 'test-issue1-route',
  //  icon: 'pets',
  //  tooltip: 'issues'
  //})
  //
  api.addTask({
    name: 'demo-task1',
    command: "echo task2!",
    // Optional
    // The rest is like `describeTask` without the `match` option
    description: '...',
    link: 'https://github.com/vuejs/vue-cli/...',
    // Parameters
    prompts: [
      {
        name: 'mode',
        type: 'list',
        default: 'development',
        choices: [
          {
            name: 'Development mode',
            value: 'development'
          },
          {
            name: 'Production mode',
            value: 'production'
          },
          {
            name: 'Test mode',
            value: 'test'
          }
        ],
        description: 'Build mode',
        link: 'https://link-to-docs'
      },
      {
        name: 'host',
        type: 'input',
        default: '0.0.0.0',
        description: 'Host for the development server'
      }
    ],
    onBeforeRun: () => {},
    onRun: () => {},
    onExit: () => {},
    //views: [{
    //  id: 'vue-issue-demo1-addon',
    //  label: 'Dashboard',
    //  icon: 'dashboard',
    //  component: 'vue-issue-demo-comp'
    //}],
    //defaultView: 'vue-issue-demo1-addon'
  })
  //
  //// Add a test page below 'plugins', 'configurations' and 'tasks' on the left sidebar
  //api.addView({
  //  id: 'org.vue.webpack.views.test2',
  //  name: 'org.vue.webpack.routes.test2',
  //  icon: 'pets',
  //  // icon: '/public/webpack-logo.png',
  //  tooltip: 'test2 view from webpack addon'
  //})

  // Shared data example to store a value (for example a setting)
  api.onProjectOpen(() => {
    //api.setSharedData('test-data', api.storageGet('vue-cli.test2-data'))
  })
  api.watchSharedData('test2-data', value => {
    api.storageSet('vue-cli.test2-data', value)
    console.log('test2-data value saved')
  })

  // Test Plugin Action
  api.onAction('test2-action', params => {
    console.log('test2-action called', params)

    setTimeout(() => {
      api.callAction('other-action', { foo: 'bar' })
    }, 1000)

    return 'meow'
  })

  // Hooks
  api.onViewOpen(({ view }) => {
    console.log('onViewOpen', view.id)
  })
  api.onTaskOpen(({ task }) => {
    console.log('onTaskOpen', task.id)
  })
  api.onTaskRun(({ task }) => {
    console.log('onTaskRun', task.id)
  })
  api.onTaskExit(({ task }) => {
    console.log('onTaskExit', task.id)
  })
  api.onConfigRead(({ config }) => {
    console.log('onConfigRead', config.id)
  })
  api.onConfigWrite(({ config }) => {
    console.log('onConfigWrite', config.id)
  })
}
