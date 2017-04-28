#ifndef tmplORM_MYSQL__HXX
#define tmplORM_MYSQL__HXX

#include "tmplORM.hxx"
#include <type_traits>
#include <memory>
#include "mysql.hxx"
#include "managedPtr.hxx"

namespace tmplORM
{
	namespace mysql
	{
		using namespace tmplORM::common;
		using namespace tmplORM::mysql::driver;
		using namespace tmplORM::types::baseTypes;

		using tmplORM::types::type_t;
		using tmplORM::types::unicode_t;
		using tmplORM::types::unicodeText_t;
		using tmplORM::types::dateTimeTypes::_dateTime_t;

		using tmplORM::types::primary_t;
		using tmplORM::types::autoInc_t;
		using tmplORM::types::nullable_t;

		// If we don't know how to translate the type, don't.
		template<typename> struct stringType_t { using value = typestring<>; };
		template<> struct stringType_t<int8_t> { using value = ts("TINYINT"); };
		template<> struct stringType_t<uint8_t> { using value = ts("TINYINT UNSIGNED"); };
		template<> struct stringType_t<int16_t> { using value = ts("SHORT"); };
		template<> struct stringType_t<uint16_t> { using value = ts("SHORT UNSIGNED"); };
		template<> struct stringType_t<int32_t> { using value = ts("INT"); };
		template<> struct stringType_t<uint32_t> { using value = ts("INT UNSIGNED"); };
		template<> struct stringType_t<int64_t> { using value = ts("BIGINT"); };
		template<> struct stringType_t<uint64_t> { using value = ts("BIGINT UNSIGNED"); };
		template<> struct stringType_t<bool> { using value = ts("BIT(1)"); };
		template<> struct stringType_t<float> { using value = ts("FLOAT"); };
		template<> struct stringType_t<double> { using value = ts("DOUBLE"); };
		template<> struct stringType_t<char *> { using value = ts("TEXT"); };
		template<> struct stringType_t<void *> { using value = ts("BLOB"); };
		template<> struct stringType_t<ormDateTime_t> { using value = ts("DATETIME"); };
//		template<> struct stringType_t<ormUUID_t> { using value = ts("VARCHAR(36)"); };
		template<typename T> using stringType = typename stringType_t<T>::value;

		template<typename> struct bind_t { };
		template<> struct bind_t<int8_t> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_TINY; };
		template<> struct bind_t<int16_t> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_SHORT; };
		template<> struct bind_t<int32_t> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_LONG; };
		template<> struct bind_t<int64_t> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_LONGLONG; };
		//
		template<> struct bind_t<bool> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_BIT; };
		template<> struct bind_t<float> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_FLOAT; };
		template<> struct bind_t<double> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_DOUBLE; };
		template<> struct bind_t<char *> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_STRING; };
		template<> struct bind_t<void *> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_BLOB; };
		template<> struct bind_t<_dateTime_t> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_DATETIME; };
		template<> struct bind_t<nullptr_t> { constexpr static const mySQLFieldType_t value = MYSQL_TYPE_NULL; };
		template<typename T, bool = isNumeric<T>::value> struct bindType_t : public bind_t<T> { };
		template<typename T> struct bindType_t<T, true> : public bind_t<typename std::make_signed<T>::type> { };

		namespace driver
		{
			using namespace tmplORM::types::baseTypes;

			static const char nullParam = char(true);
			constexpr static char *const notNullParam = nullptr;

			template<typename T> typename std::enable_if<!isNumeric<T>::value>::type
				bindT(MYSQL_BIND &param) noexcept { param.is_unsigned = false; }
			template<typename T> typename std::enable_if<isNumeric<T>::value>::type
				bindT(MYSQL_BIND &param) noexcept { param.is_unsigned = std::is_unsigned<T>::value; }

			template<bool> struct bindValue_t
			{
				using nanoseconds_t = std::chrono::nanoseconds;

				template<typename T> void operator ()(MYSQL_BIND &param, T &value, managedPtr_t<void> &) noexcept
					{ param.buffer = const_cast<T *>(&value); }

				void operator ()(MYSQL_BIND &param, ormDateTime_t &value, managedPtr_t<void> &paramStorage) noexcept
				{
					MYSQL_TIME dateTime;
					dateTime.year = value.year();
					dateTime.month = value.month();
					dateTime.day = value.day();
					dateTime.hour = value.hour();
					dateTime.minute = value.minute();
					dateTime.second = value.second();
					dateTime.second_part = value.nanoSecond() / std::chrono::duration_cast<nanoseconds_t>(1_us).count();
					dateTime.time_type = MYSQL_TIMESTAMP_DATETIME;

					// TODO: Replace me with makeManaged<>!
					paramStorage = managedPtr_t<MYSQL_TIME>(new MYSQL_TIME(dateTime));
					param.buffer = paramStorage;
					param.buffer_length = sizeof(dateTime);
				}
			};

			template<> struct bindValue_t<true>
				{ template<typename T> void operator ()(MYSQL_BIND &param, T *value, managedPtr_t<void> &) noexcept { param.buffer = value; } };

			template<typename T> void mySQLPreparedQuery_t::bind(const size_t index, const T &value, const fieldLength_t length) noexcept
			{
				if (index >= numParams)
					return;
				MYSQL_BIND &param = params[index];
				param.buffer_type = bindType_t<T>::value;
				param.buffer_length = length.first;
				bindValue_t<std::is_pointer<T>::value>()(param, const_cast<T &>(value), paramStorage[index]);
				param.length = &param.buffer_length;
				param.is_null = notNullParam;
				bindT<T>(param);
			}

			template<typename T> void mySQLPreparedQuery_t::bind(const size_t index, const nullptr_t, const fieldLength_t length) noexcept
			{
				if (index >= numParams)
					return;
				MYSQL_BIND &param = params[index];
				param.buffer_type = bindType_t<T>::value;
				param.is_null = const_cast<char *>(&nullParam);
			}
		}

		template<typename name> using backtick = tycat<ts("`"), name, ts("`")>;

		template<size_t, typename> struct fieldName_t { };
		template<size_t N, typename fieldName, typename T> struct fieldName_t<N, type_t<fieldName, T>>
			{ using value = tycat<backtick<fieldName>, comma<N>>; };

		//template<typename fieldName, typename T> auto toFieldName(const type_t<fieldName, T> &) -> fieldName;

		template<typename> struct createName_t { };
		template<typename fieldName, typename T> struct createName_t<type_t<fieldName, T>>
			{ using value = tycat<backtick<fieldName>, ts(" "), stringType<T>>; };
		template<typename fieldName, uint32_t length> struct createName_t<unicode_t<fieldName, length>>
			{ using value = tycat<backtick<fieldName>, ts(" VARCHAR("), toTypestring<length>, ts(")")>; };

#include "tmplORM.common.hxx"

		template<size_t N, typename field> struct createList__t
		{
			template<typename fieldName, typename T> static auto _name(const type_t<fieldName, T> &) ->
				typename createName_t<type_t<fieldName, T>>::value;
			template<typename fieldName, uint32_t length> static auto _name(const unicode_t<fieldName, length> &) ->
				typename createName_t<unicode_t<fieldName, length>>::value;
			template<typename T> static auto _name(const primary_t<T> &) -> tycat<decltype(_name(T())), ts(" PRIMARY KEY")>;
			template<typename T> static auto _name(const autoInc_t<T> &) -> tycat<decltype(_name(T())), ts(" AUTO_INCREMENT")>;
			using name = decltype(_name(field()));

			static auto value() -> tycat<name, nullable<field::nullable>, comma<N>>;
		};
		template<size_t N, typename T> using createList__ = decltype(createList__t<N, T>::value());

		template<size_t N, typename field, typename... fields> struct createList_t
			{ using value = tycat<createList__<N, field>, typename createList_t<N - 1, fields...>::value>; };
		template<typename field> struct createList_t<1, field> { using value = createList__<1, field>; };
		// Alias to make the above easier to use
		template<typename... fields> using createList = typename createList_t<sizeof...(fields), fields...>::value;

		template<typename tableName, typename... fields> using createTable_ = toString<
			tycat<ts("CREATE TABLE "), backtick<tableName>, ts(" ("), createList<fields...>, ts(");")>
		>;
		template<typename tableName, typename... fields> using select_ = toString<
			tycat<ts("SELECT "), selectList<fields...>, ts(" FROM "), backtick<tableName>, ts(";")>
		>;
		template<typename tableName, typename... fields> using add_ = toString<
			tycat<ts("INSERT INTO "), backtick<tableName>, ts(" ("), insertList<fields...>, ts(") VALUES ("), placeholder<countInsert_t<fields...>::count>, ts(");")>
		>;
		template<typename tableName, typename... fields> struct update_t<false, tableName, fields...>
			{ using value = tycat<ts("UPDATE "), backtick<tableName>, ts(" SET "), updateList<fields...>, updateWhere<fields...>, ts(";")>; };
		template<typename tableName, typename... fields> using del_ = toString<
			tycat<ts("DELETE FROM "), backtick<tableName>, updateWhere<fields...>, ts(";")>
		>;
		template<typename tableName> using deleteTable_ = toString<
			tycat<ts("DROP TABLE "), backtick<tableName>, ts(";")>
		>;

		struct session_t final
		{
		private:
			driver::mySQLClient_t database;

		public:
			session_t() noexcept : database() { }
			~session_t() noexcept { }

			template<typename tableName, typename... fields> bool createTable(const model_t<tableName, fields...> &) noexcept
			{
				using create = createTable_<tableName, fields...>;
				return database.query(create::value);
			}

			template<typename T, typename tableName, typename... fields_t> fixedVector_t<T> select(const model_t<tableName, fields_t...> &) noexcept
			{
				using select = select_<tableName, fields_t...>;
				if (database.query(select::value))
				{
					mySQLResult_t result = database.queryResult();
					fixedVector_t<T> values(result.numRows());
					mySQLRow_t row = result.resultRows();
					for (size_t i = 0; i < values.length(); ++i)
					{
						T value;
						if (!row.valid())
							return {}; // TODO: Better sanity check here..
						bindSelect<fields_t...>::bind(value.fields(), row);
						values[i] = std::move(value);
						row.next();
					}
					if (row.valid())
						return {}; // TODO: Better sanity check here..
					return values;
				}
				return {};
			}

			template<typename tableName, typename... fields_t> bool add(const model_t<tableName, fields_t...> &model) noexcept
			{
				using insert = add_<tableName, fields_t...>;
				mySQLPreparedQuery_t query = database.prepare(insert::value, countInsert_t<fields_t...>::count);
				bindInsert<fields_t...>::bind(model.fields(), query);
				if (query.execute())
				{
					if (hasAutoInc<fields_t...>())
						getAutoInc(model) = query.rowID();
					return true;
				}
				return false;
			}

			template<typename tableName, typename... fields_t> bool update(const model_t<tableName, fields_t...> &model) noexcept
			{
				using update = update_<tableName, fields_t...>;
				if (std::is_same<update, toString<typestring<>>>::value)
					return false;
				mySQLPreparedQuery_t query(database.prepare(update::value, sizeof...(fields_t)));
				// This binds the fields, primary key last so it tags to the WHERE clause for this query.
				bindUpdate<fields_t...>::bind(model.fields(), query);
				return query.execute();
			}

			template<typename tableName, typename... fields_t> bool del(const model_t<tableName, fields_t...> &model) noexcept
			{
				using del = del_<tableName, fields_t...>;
				mySQLPreparedQuery_t query(database.prepare(del::value, countPrimary<fields_t...>::count));
				// This binds just the primary keys of the model so it tags in-order to the WHERE clause for this query.
				bindDelete<fields_t...>::bind(model.fields(), query);
				return query.execute();
			}

			template<typename tableName, typename... fields> bool deleteTable(const model_t<tableName, fields...> &) noexcept
			{
				using deleteTable = deleteTable_<tableName>;
				return database.query(deleteTable::value);
			}
		};
	}
	using mysql_t = mysql::session_t;
}

#endif /*tmplORM_MYSQL__HXX*/
