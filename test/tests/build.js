var mill = require('../..');

module.exports = function (t) {

  t.test('build',function(t){

    t.plan(1);

    mill.trace();

    t.ok((mill.test() === undefined), 'built');
  })
}