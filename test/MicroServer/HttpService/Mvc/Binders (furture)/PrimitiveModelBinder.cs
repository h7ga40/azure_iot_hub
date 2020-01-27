using MicroServer.Utilities;
using System;

namespace MicroServer.Net.Http.Binders
{
	/// <summary>
	/// Can bind primitives and <c>string</c>.
	/// </summary>
	public class PrimitiveModelBinder : IModelBinder
	{
		/// <summary>
		/// Determines whether this instance can bind the specified model.
		/// </summary>
		/// <param name="context">Context information.</param>
		/// <returns>
		///   <c>true</c> if this instance can handle the model; otherwise <c>false</c>.
		/// </returns>
		public bool CanBind(IModelBinderContext context)
		{
			return context.ModelType == typeof(string)
				|| context.ModelType == typeof(bool)
				|| context.ModelType == typeof(byte)
				|| context.ModelType == typeof(sbyte)
				|| context.ModelType == typeof(short)
				|| context.ModelType == typeof(ushort)
				|| context.ModelType == typeof(int)
				|| context.ModelType == typeof(uint)
				|| context.ModelType == typeof(long)
				|| context.ModelType == typeof(ulong)
				|| context.ModelType == typeof(double);
		}

		/// <summary>
		/// Bind the model
		/// </summary>
		/// <param name="context">Context information</param>
		/// <returns>
		/// An object of the specified type (<seealso cref="IModelBinderContext.ModelType)" />
		/// </returns>
		public object Bind(IModelBinderContext context)
		{
			var name = StringUtility.IsNullOrEmpty(context.Prefix)
						   ? context.ModelName
						   : context.Prefix + "." + context.ModelName;
			var parameter = context.ValueProvider.Get(name);
			if (parameter == null)
				return null;

			object value = parameter.Value;

			try {
				if (context.ModelType == typeof(string)) {
					return value;
				}

				if (context.ModelType == typeof(int)) {
					return value = Int32.Parse((string)value);
				}

				if (context.ModelType == typeof(bool)) {

					if ((string)value == "1" || ((string)value).ToUpper() == Boolean.TrueString.ToUpper()) {
						return true;
					}
					else if ((string)value == "0" || ((string)value).ToUpper() == Boolean.FalseString.ToUpper()) {
						return false;
					}

					return null;
				}

				if (context.ModelType == typeof(byte)) {
					return value = Byte.Parse((string)value);
				}

				if (context.ModelType == typeof(sbyte)) {
					return value = SByte.Parse((string)value);
				}

				if (context.ModelType == typeof(short)) {
					return value = Int16.Parse((string)value);
				}

				if (context.ModelType == typeof(ushort)) {
					return value = UInt16.Parse((string)value);
				}

				if (context.ModelType == typeof(int)) {
					return value = Int32.Parse((string)value);
				}

				if (context.ModelType == typeof(uint)) {
					return value = UInt32.Parse((string)value);
				}

				if (context.ModelType == typeof(long)) {
					return value = Int64.Parse((string)value);
				}

				if (context.ModelType == typeof(ulong)) {
					return value = UInt64.Parse((string)value);
				}

				if (context.ModelType == typeof(double)) {
					return value = Double.Parse((string)value);
				}

			}
			catch (Exception ex) {
				throw new ModelBindingException(ex.Message, context.ModelName, value);
			}

			return null;
		}
	}
}