using System;
using System.Collections;
using System.Reflection;

namespace MicroServer.Net.Http.Mvc.Controllers
{
	public class ControllerIndexer
	{
		private ArrayList _controllers = new ArrayList();

		public IEnumerable Controllers => _controllers;

		public void Find()
		{
			foreach (var assembly in AppDomain.CurrentDomain.GetAssemblies()) {
				Find(assembly);
			}
		}

		public void Find(Assembly assembly)
		{
			var controllerType = typeof(Controller);
			foreach (var type in assembly.GetTypes()) {
				if (type.IsAbstract || type.IsInterface || type.IsNotPublic)
					continue;

				if (type.IsSubclassOf(controllerType))
					MapController(type);
			}
		}

		private void MapController(Type type)
		{
			var mapping = new ControllerMapping(type) {
				Uri = type.Name.Replace("Controller", String.Empty)
			};

			foreach (var method in type.GetMethods()) {
				if (type.IsAbstract || type.IsNotPublic)
					continue;

				if (method.ReturnType.Equals(typeof(IActionResult)))
					mapping.Add(method);
			}
			_controllers.Add(mapping);
		}
	}
}
