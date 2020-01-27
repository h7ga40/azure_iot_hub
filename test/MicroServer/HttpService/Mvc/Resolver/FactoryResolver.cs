using System;

namespace MicroServer.Net.Http.Mvc.Resolver
{
	/// <summary>
	/// Use this get constructor info.
	/// </summary>
	public class FactoryResolver : IServiceResolver
	{
		public object Resolve(Type type)
		{
			var constructorInfo = type.GetConstructor(new Type[] { });
			return constructorInfo.Invoke(new object[] { });
		}
	}
}
